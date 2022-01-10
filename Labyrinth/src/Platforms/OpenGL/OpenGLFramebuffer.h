#pragma once

#include "Labyrinth/Renderer/Framebuffer.h"

namespace Labyrinth {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual uint32_t getColorAttachmentRendererID() const override { return mColorAttachment; }

		virtual const FramebufferSpec& getSpecification() const override { return mSpecification; }
	private:
		uint32_t mRendererID;
		uint32_t mColorAttachment, mDepthAttachment;
		FramebufferSpec mSpecification;
	};

}