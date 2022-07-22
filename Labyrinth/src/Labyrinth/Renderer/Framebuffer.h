#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <vector>

namespace Labyrinth {

	enum class LAB_API FramebufferTextureFormat
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

	struct LAB_API FramebufferTextureSpec
	{
		FramebufferTextureSpec() = default;
		FramebufferTextureSpec(FramebufferTextureFormat format)
			: textureFormat(format) {}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct LAB_API FramebufferAttachmentSpec
	{
		FramebufferAttachmentSpec() = default;
		FramebufferAttachmentSpec(std::initializer_list<FramebufferTextureSpec> attachment_list)
			: attachments(attachment_list) {}

		std::vector<FramebufferTextureSpec> attachments;
	};

	struct LAB_API FramebufferSpec
	{
		i32 width, height;
		FramebufferAttachmentSpec attachments;
		u32 samples = 1;

		bool swapChainTarget = true;
	};

	class LAB_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(usize width, usize height) = 0;
		virtual i32 readPixel(u32 attachmentIndex, i32 x, i32 y) = 0;
		virtual void readData(u32 attachmentIndex, void* data) = 0;

		virtual void clearAttachment(u32 attachmentIndex, i32 value) = 0;

		virtual void bindColourAttachment(u32 index = 0) = 0;
		virtual u32 getColourAttachmentRendererID(u32 index = 0) const = 0;

		virtual const FramebufferSpec& getSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpec& spec);
	};
}