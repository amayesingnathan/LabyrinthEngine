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
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override { LAB_CORE_ASSERT(index < mColourAttachments.size()); return mColourAttachments[index]; }

		virtual const FramebufferSpec& getSpecification() const override { return mSpecification; }
	private:
		uint32_t mRendererID = 0;
		FramebufferSpec mSpecification;

		std::vector<FramebufferTextureSpec> mColourAttachmentSpecs;
		FramebufferTextureSpec mDepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> mColourAttachments;
		uint32_t mDepthAttachment = 0;
	};

}