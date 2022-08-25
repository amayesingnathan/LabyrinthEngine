#include "Lpch.h"
#include "TextureSlot.h"

#include "Renderer.h"
#include "OpenGL/OpenGLTextureSlot.h"

namespace Labyrinth {

	Ref<TextureSlot> TextureSlot::Create(const Ref<Texture2D>& tex)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureSlot>::Create(tex);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureSlot> TextureSlot::Create(const Ref<Framebuffer>& fbo)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LAB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureSlot>::Create(fbo);
		}

		LAB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}