#pragma once

#include "Labyrinth/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <string>

typedef unsigned int GLenum;

namespace Labyrinth {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		void uploadUniformInt(const std::string& name, int value);

		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void uploadUniformMat3(const std::string& name, const glm::mat3& value);
		void uploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		int GetUniformLocation(const std::string& name) const;

		std::string ReadFile(const std::string filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t mRendererID;
		mutable std::unordered_map<std::string, int> mUniformLocCache;
	};

}