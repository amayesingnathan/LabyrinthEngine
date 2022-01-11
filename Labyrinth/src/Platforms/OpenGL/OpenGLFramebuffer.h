#pragma once

#include "Labyrinth/Renderer/Framebuffer.h"

namespace Labyrinth {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		virtual ~OpenGLFramebuffer() override;

		void Invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual uint32_t getColorAttachmentRendererID() const override { return mColorAttachment; }

		virtual const FramebufferSpec& getSpecification() const override { return mSpecification; }
	private:
		uint32_t mRendererID = 0;
		uint32_t mColorAttachment = 0, mDepthAttachment = 0;
		FramebufferSpec mSpecification;
	};

}