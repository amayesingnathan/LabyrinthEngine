#pragma once

#include "Labyrinth/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <string>

typedef u32 GLenum;

namespace Labyrinth {

	class LAB_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const std::string& getName() const override { return mName; }

		void uploadUniformInt(const std::string& name, i32 value);
		void uploadUniformIntArray(const std::string& name, i32* values, u32 count);

		void uploadUniformFloat(const std::string& name, f32 value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void uploadUniformMat3(const std::string& name, const glm::mat3& value);
		void uploadUniformMat4(const std::string& name, const glm::mat4& value);

		virtual void setInt(const std::string& name, i32 value) override;
		virtual void setIntArray(const std::string& name, i32* values, u32 count) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;
		virtual void setFloat(const std::string& name, f32 value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;

	private:
		i32 GetUniformLocation(const std::string& name) const;

		std::string ReadFile(const std::string filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileOrGetOpenGLBinaries();
		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<u32>& shaderData);

	private:
		u32 mRendererID;

		std::string mFilepath;
		std::string mName;

		std::unordered_map<GLenum, std::vector<u32>> mVulkanSPIRV;
		std::unordered_map<GLenum, std::vector<u32>> mOpenGLSPIRV;

		std::unordered_map<GLenum, std::string> mOpenGLSourceCode;

		mutable std::unordered_map<std::string, i32> mUniformLocCache;
	};

}