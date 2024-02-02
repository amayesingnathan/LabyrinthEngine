#pragma once

#include <Labyrinth/Core/System/Ref.h>

typedef unsigned int GLenum;

namespace Laby {

	class Shader : public RefCounted
	{
	public:
		Shader(const std::string& filepath);
		Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		const std::string& GetName() const { return mName; }

		void SetInt(const std::string& name, i32 value);
		void SetIntArray(const std::string& name, i32* values, u32 count);
		void SetMat4(const std::string& name, const glm::mat4& value);
		void SetFloat(const std::string& name, f32 value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);

	private:
		i32 GetUniformLocation(const std::string& name) const;

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

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		Ref<Shader> Load(const std::string& filepath);

		Ref<Shader> Select(const std::string& name);

		bool Exists(const std::string& name) { return mShaders.contains(name); }

	private:
		std::unordered_map<std::string, Ref<Shader>> mShaders;
	};

}