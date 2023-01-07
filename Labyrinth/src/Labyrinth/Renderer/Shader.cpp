#include "Lpch.h"
#include "Shader.h"

#include "Labyrinth/IO/Filesystem.h"
#include "Labyrinth/Tools/Timing.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Laby {

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			LAB_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			LAB_CORE_ASSERT(false);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			LAB_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "resources/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(u32 stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			LAB_CORE_ASSERT(false);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(u32 stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			LAB_CORE_ASSERT(false);
			return "";
		}
	}

	Shader::Shader(const std::string& filepath)
		: mFilepath(filepath)
	{
		Utils::CreateCacheDirectoryIfNeeded();

		std::string source;
		FileUtils::Read(filepath, source);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			LAB_CORE_WARN("Shader creation took {0} ms", timer.elapsedMillis());
		}

		//Extract name from file path
		auto nameStart = filepath.find_last_of('/\\');
		nameStart = nameStart == std::string::npos ? 0 : nameStart + 1;
		auto nameEnd = filepath.rfind('.');
		auto count = (nameEnd == std::string::npos) ? filepath.size() - nameEnd : nameEnd - nameStart;
		mName = filepath.substr(nameStart, count);
	}

	Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: mName(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}

	Shader::~Shader()
	{
		glDeleteProgram(mRendererID);
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		usize typeTokenLength = strlen(typeToken);
		usize pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			usize eol = source.find_first_of("\r\n", pos);
			LAB_CORE_ASSERT(eol != std::string::npos, "Shader syntax error!");
			usize begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			LAB_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type given!");

			usize nextLinePos = source.find_first_not_of("\r\n", eol);
			LAB_CORE_ASSERT(nextLinePos != std::string::npos, "Shader syntax error!");
			pos = source.find(typeToken, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void Shader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = mVulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = mFilepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(u32));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), mFilepath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					LAB_CORE_ERROR(module.GetErrorMessage());
					LAB_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<u32>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(u32));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void Shader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = mOpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		mOpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : mVulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = mFilepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(u32));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				mOpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = mOpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), mFilepath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					LAB_CORE_ERROR(module.GetErrorMessage());
					LAB_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<u32>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(u32));
					out.flush();
					out.close();
				}
			}
		}
	}

	void Shader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : mOpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (u32)(spirv.size() * sizeof(u32)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			LAB_CORE_ERROR("Shader linking failed ({0}):\n{1}", mFilepath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		mRendererID = program;
	}

	void Shader::Reflect(GLenum stage, const std::vector<u32>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		LAB_CORE_TRACE("Shader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), mFilepath);
		LAB_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		LAB_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		LAB_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			usize bufferSize = compiler.get_declared_struct_size(bufferType);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			usize memberCount = bufferType.member_types.size();

			LAB_CORE_TRACE("  {0}", resource.name);
			LAB_CORE_TRACE("    Size = {0}", bufferSize);
			LAB_CORE_TRACE("    Binding = {0}", binding);
			LAB_CORE_TRACE("    Members = {0}", memberCount);
		}
	}

	void Shader::bind() const
	{
		glUseProgram(mRendererID);
	}

	void Shader::unbind() const
	{
		glUseProgram(0);
	}

	void Shader::setInt(const std::string& name, i32 value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform1i(location, value);
	}

	void Shader::setIntArray(const std::string& name, i32* values, u32 count)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void Shader::setMat4(const std::string& name, const glm::mat4& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::setFloat(const std::string& name, f32 value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform1f(location, value);
	}

	void Shader::setFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::setFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		if (location == -1) return;

		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	GLint Shader::GetUniformLocation(const std::string& name) const
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


	/*
		Shader Library
	*/

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
	{
		LAB_CORE_ASSERT(!exists(name), "Shader already exists!");
		mShaders[name] = shader;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader)
	{
		auto& name = shader->getName();
		add(name, shader);
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		Ref<Shader> shader = Ref<Shader>::Create(filepath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		Ref<Shader> shader = Ref<Shader>::Create(filepath);
		add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::select(const std::string& name)
	{
		LAB_CORE_ASSERT(exists(name), "Shader does not exist!");
		return mShaders[name];
	}
}