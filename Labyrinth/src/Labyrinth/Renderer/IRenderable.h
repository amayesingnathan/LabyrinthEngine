#pragma once

#include "Labyrinth/Core/System/Ref.h"

namespace Laby {

	class IRenderable : public virtual RefCounted
	{
	public:
		virtual u32 getTextureID() const = 0;
		virtual void bindTexture(u32 slot = 0) const = 0;
		virtual const glm::vec2* getTextureCoords() const { return nullptr; }

		bool operator==(const IRenderable& other) const { return getTextureID() == other.getTextureID(); }
	};
}