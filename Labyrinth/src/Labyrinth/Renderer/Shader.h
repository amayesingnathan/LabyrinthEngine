#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual const std::string& getName() const = 0;

		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string fragmentSrc);
	};

	class ShaderLibrary
	{
	public:
		void add(const std::string& name, const Ref<Shader>& shader);
		void add(const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& name, const std::string& filepath);
		Ref<Shader> load(const std::string& filepath);

		Ref<Shader> select(const std::string& name);

		bool exists(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> mShaders;
	};

}