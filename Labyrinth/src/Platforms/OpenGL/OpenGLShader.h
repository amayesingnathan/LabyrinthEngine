#pragma once

#include "Labyrinth/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <string>

namespace Labyrinth {

	class OpenGLShader : public Shader
	{
	public:
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
		GLint getUniformLocation(const std::string& name) const;

	private:
		uint32_t mRendererID;
		mutable std::unordered_map<std::string, GLint> mUniformLocCache;
	};

}