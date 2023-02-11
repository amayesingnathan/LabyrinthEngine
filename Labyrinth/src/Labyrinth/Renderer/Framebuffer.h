#pragma once

#include "IRenderable.h"

namespace Laby {

	enum class FramebufferTextureFormat
	{
		None = 0,

		// Colour
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpec
	{
		FramebufferTextureSpec() = default;
		FramebufferTextureSpec(FramebufferTextureFormat format)
			: textureFormat(format) {}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpec
	{
		FramebufferAttachmentSpec() = default;
		FramebufferAttachmentSpec(std::initializer_list<FramebufferTextureSpec> attachment_list)
			: attachments(attachment_list) {}

		std::vector<FramebufferTextureSpec> attachments;
	};

	struct FramebufferSpec
	{
		i32 width, height;
		FramebufferAttachmentSpec attachments;
		u32 samples = 1;

		bool swapChainTarget = true;
	};

	class Framebuffer : public IRenderable
	{
	public:
		Framebuffer(const FramebufferSpec& spec);
		~Framebuffer();

		void bind();
		void unbind();

		u32 getTextureID() const override { return getColourAttachmentRendererID(); }

		void resize(usize width, usize height);
		i32 readPixel(u32 attachmentIndex, i32 x, i32 y) const;

		void clearAttachment(u32 attachmentIndex, i32 value);

		void bindColourAttachment(u32 index = 0) const;
		u32 getColourAttachmentRendererID(u32 index = 0) const
		{
			LAB_CORE_ASSERT(index < mColourAttachments.size());
			return mColourAttachments[index];
		}

		const FramebufferSpec& getSpecification() const { return mSpecification; }

	private:
		void Invalidate();

	private:
		u32 mRendererID = 0;
		FramebufferSpec mSpecification;

		std::vector<FramebufferTextureSpec> mColourAttachmentSpecs;
		FramebufferTextureSpec mDepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<u32> mColourAttachments;
		u32 mDepthAttachment = 0;
	};

}