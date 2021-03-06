#pragma once

#include "Labyrinth/Renderer/Framebuffer.h"

namespace Labyrinth {

	class LAB_API OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		virtual ~OpenGLFramebuffer() override;

		void Invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(size_t width, size_t height) override;
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void readData(uint32_t attachmentIndex, void* data) override;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

		virtual void bindColourAttachment(uint32_t index = 0);
		virtual uint32_t getColourAttachmentRendererID(uint32_t index = 0) const override 
		{ 
			LAB_CORE_ASSERT(index < mColourAttachments.size()); 
			return mColourAttachments[index]; 
		}

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