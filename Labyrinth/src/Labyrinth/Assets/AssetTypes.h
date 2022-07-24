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
		Group
	};

	namespace AssetUtils {

		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			else if (assetType == "Scene")		return AssetType::Scene;
			else if (assetType == "Texture")	return AssetType::Texture;
			else if (assetType == "Group")		return AssetType::Group;

			LAB_CORE_ASSERT(false, "Unknown Asset Type!");
			return AssetType::None;
		}

		inline const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case AssetType::None:       return "None";
			case AssetType::Scene:      return "Scene";
			case AssetType::Texture:    return "Texture";
			case AssetType::Group:		return "Group";
			}

			LAB_CORE_ASSERT(false, "Unknown Asset Type");
			return "None";
		}
	}


}