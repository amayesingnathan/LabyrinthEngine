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

		virtual void resize(usize width, usize height) override;
		virtual i32 readPixel(u32 attachmentIndex, i32 x, i32 y) const override;
		virtual Buffer readData(u32 attachmentIndex) const override;

		virtual void clearAttachment(u32 attachmentIndex, i32 value) override;

		virtual void bindColourAttachment(u32 index = 0);
		virtual u32 getColourAttachmentRendererID(u32 index = 0) const override 
		{ 
			LAB_CORE_ASSERT(index < mColourAttachments.size()); 
			return mColourAttachments[index]; 
		}

		virtual const FramebufferSpec& getSpecification() const override { return mSpecification; }

		virtual Ref<Texture2D> toTex() const override;

	private:
		u32 mRendererID = 0;
		FramebufferSpec mSpecification;

		std::vector<FramebufferTextureSpec> mColourAttachmentSpecs;
		FramebufferTextureSpec mDepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<u32> mColourAttachments;
		u32 mDepthAttachment = 0;
	};

}