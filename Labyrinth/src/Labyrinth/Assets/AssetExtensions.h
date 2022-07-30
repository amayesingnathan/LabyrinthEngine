#pragma once

#include <unordered_map>

#include "AssetTypes.h"

namespace Labyrinth {

	inline static std::unordered_map<std::string, AssetType> sAssetExtensionMap =
	{
		// Hazel types
		{ ".laby", AssetType::Scene },

		// Textures
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
	};

}
