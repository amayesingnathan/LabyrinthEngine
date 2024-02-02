#pragma once

#include "Labyrinth/Core/System/Ref.h"

namespace Laby {

	enum class RenderType
	{
		None, Texture, SubTexture, Tilemap
	};

	class IRenderable : public virtual RefCounted
	{
	public:
		virtual u32 GetTextureID() const = 0;
		void BindTexture(u32 slot = 0) const;
		virtual const glm::vec2* GetTextureCoords() const { return DefaultCoords; }

		bool operator==(const IRenderable& other) const { return GetTextureID() == other.GetTextureID(); }

	private:
		static constexpr glm::vec2 DefaultCoords[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };
	};
}