#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	using TileID = usize;
	static constexpr TileID NullTileID = Limits<usize>::Max;

	struct TileRenderData
	{
		TileID textureID = NullTileID;
		f32 rotation = 0.0f;

		TileRenderData() = default;
		TileRenderData(TileID id, f32 r = 0.0f) : textureID(id), rotation(r) {}
		bool valid() const { return textureID != NullTileID; }
	};
}