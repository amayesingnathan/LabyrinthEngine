#pragma once

#include "Labyrinth/Core/Base.h"

namespace Labyrinth {

	struct FramebufferSpec
	{
		uint32_t width, height;
		uint32_t samples = 1;

		bool swapChainTarget = true;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t getColorAttachmentRendererID() const = 0;

		virtual const FramebufferSpec& getSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpec& spec);
	};
}