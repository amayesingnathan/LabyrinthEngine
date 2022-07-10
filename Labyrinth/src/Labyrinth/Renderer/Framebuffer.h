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
		size_t width, height;
		FramebufferAttachmentSpec attachments;
		uint32_t samples = 1;

		bool swapChainTarget = true;
	};

	class LAB_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(size_t width, size_t height) = 0;
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual void readData(uint32_t attachmentIndex, void* data) = 0;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual void bindColourAttachment(uint32_t index = 0) = 0;
		virtual uint32_t getColourAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpec& getSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpec& spec);
	};
}