#pragma once

#include <glm/glm.hpp>

#include "Labyrinth/Core/System/Ref.h"

namespace Laby {

	class IRenderable : public virtual RefCounted
	{
	public:
		virtual u32 getRendererID() const = 0;
		virtual void bind(u32 slot = 0) const = 0;
		virtual const glm::vec2* getTextureCoords() const { return nullptr; }

		bool operator==(const IRenderable& other) const { return getRendererID() == other.getRendererID(); }
	};
}