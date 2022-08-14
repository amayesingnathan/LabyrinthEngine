#pragma once

#include "Tilemap.h"

#include <Labyrinth/Core/System/Types.h>

namespace Labyrinth {

	struct TileComponent
	{
		Tile tile;

		TileComponent(usize x, usize y) : tile(x, y) {}
	};

}