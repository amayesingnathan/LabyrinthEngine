#pragma once

#include <Labyrinth/Core/System/Assert.h>

namespace Labyrinth {

	enum class AssetFlag : u16
	{
		None = 0,
		Missing = BIT(0),
		Invalid = BIT(1)
	};

	enum class AssetType : u16
	{
		None = 0,
		Scene,
		Texture,
		SubTexture,
		TextureSheet,
		Tilemap
	};
}