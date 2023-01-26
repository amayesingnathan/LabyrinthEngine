#pragma once

#include <unordered_map>

#include "AssetTypes.h"

namespace Laby {

	inline static std::unordered_map<std::string_view, AssetType> sAssetExtensionMap =
	{
		// Labyrinth types
		{ ".lscene", AssetType::Scene },
		{ ".lss", AssetType::TextureSheet },
		{ ".lstex", AssetType::SubTexture },
		{ ".lmap", AssetType::Tilemap },

		// Textures
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".svg", AssetType::Texture }
	};

	inline static std::unordered_map<AssetType, std::string_view> sAssetCreationMap =
	{
		{ AssetType::Scene, ".lscene" },
		{ AssetType::TextureSheet, ".lss" },
		{ AssetType::SubTexture, ".lstex" },
		{ AssetType::Tilemap, ".lmap" }
	};
}
