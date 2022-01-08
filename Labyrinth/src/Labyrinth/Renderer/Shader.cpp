#include "Lpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLShader.h"

namespace Labyrinth {

    Shader* Shader::Create(const std::string& vertexSrc, const std::string fragmentSrc)
    {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		LAB_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
    }

}