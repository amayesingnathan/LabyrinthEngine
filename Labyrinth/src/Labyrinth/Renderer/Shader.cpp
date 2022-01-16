#include "Lpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "OpenGL/OpenGLShader.h"

namespace Labyrinth {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(filepath);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string fragmentSrc)
    {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
    }

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
		Ref<Shader> shader = Shader::Create(filepath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::select(const std::string& name)
	{
		LAB_CORE_ASSERT(exists(name), "Shader does not exist!");
		return mShaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name)
	{
		return mShaders.find(name) != mShaders.end();
	}

}