#pragma once

#include "Asset.h"

namespace Labyrinth {

	struct AssetMetadata
	{
		AssetHandle handle = 0;
		AssetType type;

		std::filesystem::path filepath;
		bool dataLoaded = false;
		bool memoryAsset = false;

		bool valid() const { return handle != 0 && !memoryAsset; }
	};

}