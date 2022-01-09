#include "Lpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		LAB_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		//Extract name from file path
		auto nameStart = filepath.rfind('/');
		nameStart = nameStart == std::string::npos ? 0 : nameStart + 1;
		auto nameEnd = filepath.rfind('.');
		auto count = (nameEnd == std::string::npos) ? filepath.size() - nameEnd : nameEnd - nameStart;

		mName = filepath.substr(nameStart, count);

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: mName(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(mRendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			LAB_CORE_ERROR("Could not open file {0}", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			LAB_CORE_ASSERT(eol != std::string::npos, "Shader syntax error!");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			LAB_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type given!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			LAB_CORE_ASSERT(nextLinePos != std::string::npos, "Shader syntax error!");
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		LAB_CORE_ASSERT(shaderSources.size() <= 2, "Two shaders per file supported only.");
		std::array<GLenum, 2> glShaderIDs;
		uint8_t glShaderIndex = 0;

		for (auto& shaderData : shaderSources)
		{
			GLenum type = shaderData.first;
			const std::string source = shaderData.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				LAB_CORE_ERROR("{0}", infoLog.data());
				LAB_CORE_ASSERT(false, "Vertex shader compilation failure!");
				return;
			}

			// Attach our shaders to our program
			glAttachShader(program, shader);
			glShaderIDs[glShaderIndex++] = shader;

		}

		mRendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShaderIDs)
				glDeleteShader(id);

			LAB_CORE_ERROR("{0}", infoLog.data());
			LAB_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Don't leak shaders
		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(mRendererID);
	}

	void OpenGLShader::unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform4f(location, value.x, value.y, value.z, value. w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		if (mUniformLocCache.find(name) != mUniformLocCache.end())
			return mUniformLocCache[name];

		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		if (location == -1)
		{
			//Don't store in cache if unable to find uniform.
			LAB_CORE_ERROR("Could not get uniform `{0}` from shader to add to cache!", name);
			return location;
		}

		mUniformLocCache[name] = location;
		return location;
	}
}