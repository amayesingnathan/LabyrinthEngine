#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	enum class AssetFlag : u16
	{
		None = 0,
		Missing = LAB_BIT(0),
		Invalid = LAB_BIT(1)
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