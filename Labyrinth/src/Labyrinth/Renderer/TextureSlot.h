#pragma once

#include "Texture.h"
#include "Framebuffer.h"

#include "Labyrinth/Core/System/Base.h"

#include <string>
#include <regex>

namespace Labyrinth {

	class TextureSlot : public RefCounted
	{
	public:
		virtual ~TextureSlot() = default;

		virtual u32 getRendererID() const = 0;

		virtual void bind(u32 slot = 0) const = 0;

		virtual bool operator==(const TextureSlot& other) const = 0;

		static Ref<TextureSlot> Create(const Ref<Texture2D>& tex);
		static Ref<TextureSlot> Create(const Ref<Framebuffer>& fbo);
	};

}