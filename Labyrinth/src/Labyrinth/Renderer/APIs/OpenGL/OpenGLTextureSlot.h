#pragma once

#include <Labyrinth/Renderer/TextureSlot.h>
#include <Labyrinth/Renderer/Texture.h>
#include <Labyrinth/Renderer/Framebuffer.h>

#include <glad/glad.h>

namespace Labyrinth {

	class OpenGLTextureSlot : public TextureSlot
	{
	public:
		OpenGLTextureSlot(const Ref<Texture2D>& tex);
		OpenGLTextureSlot(const Ref<Framebuffer>& fbo);
		virtual ~OpenGLTextureSlot() {}

		virtual u32 getRendererID() const override { return mRendererID; }

		virtual void bind(u32 slot = 0) const override;

		virtual bool operator==(const TextureSlot& other) const override
		{
			return mRendererID == other.getRendererID();
		}

	private:
		u32 mRendererID;
	};

}