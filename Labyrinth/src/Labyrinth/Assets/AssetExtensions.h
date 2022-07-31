#pragma once

#include <unordered_map>

#include "AssetTypes.h"

namespace Labyrinth {

	inline static std::unordered_map<std::string, AssetType> sAssetExtensionMap =
	{
		// Labyrinth types
		{ ".lscene", AssetType::Scene },
		{ ".lss", AssetType::TextureSheet },
		{ ".lstex", AssetType::SubTexture },

		// Textures
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".svg", AssetType::Texture },
	};

}
