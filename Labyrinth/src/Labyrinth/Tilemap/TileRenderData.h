#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	using TileID = u32;
	static constexpr TileID NullTileID = Limits<TileID>::Max;

	struct TileRenderData
	{
		TileID textureID = NullTileID;
		f32 rotation = 0.0f;

		TileRenderData() = default;
		TileRenderData(TileID id, f32 r = 0.0f) : textureID(id), rotation(r) {}
		bool valid() const { return textureID != NullTileID; }

		bool operator==(const TileRenderData& other) const { return textureID == other.textureID && rotation == other.rotation; }
		bool operator!=(const TileRenderData& other) const { return !(*this == other); }
	};
}