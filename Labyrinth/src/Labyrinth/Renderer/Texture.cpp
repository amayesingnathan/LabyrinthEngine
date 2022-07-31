#include "Lpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "OpenGL/OpenGLTexture.h"

namespace Labyrinth {

	Ref<Texture2D> Texture2D::Create(i32 width, i32 height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(width, height);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(path);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}