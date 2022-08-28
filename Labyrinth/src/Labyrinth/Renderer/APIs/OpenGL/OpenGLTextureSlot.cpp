#include "Lpch.h"
#include "OpenGLTextureSlot.h"

namespace Labyrinth {

	OpenGLTextureSlot::OpenGLTextureSlot(const Ref<Texture2D>& tex)
		: mRendererID(tex->getRendererID())
	{
		LAB_PROFILE_FUNCTION();
	}

	OpenGLTextureSlot::OpenGLTextureSlot(const Ref<Framebuffer>& fbo)
		: mRendererID(fbo->getColourAttachmentRendererID())
	{
		LAB_PROFILE_FUNCTION();
	}

	void OpenGLTextureSlot::bind(u32 slot) const
	{
		LAB_PROFILE_FUNCTION();

		glBindTextureUnit(slot, mRendererID);
	}

}