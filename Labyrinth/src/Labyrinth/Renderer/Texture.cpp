#include "Lpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "OpenGL/OpenGLTexture.h"

namespace Labyrinth {

	const std::regex Texture2D::mSuppImgTypes = std::regex(".png|.svg");

	Ref<Texture2D> Texture2D::Create(size_t width, size_t height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}