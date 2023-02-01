#pragma once

#include <Labyrinth/IO/Filesystem.h>

#include "Asset.h"

namespace Laby {

	struct AssetMetadata
	{
		AssetHandle handle = 0;
		AssetType type;

		fs::path filepath;
		bool dataLoaded = false;
		bool memoryAsset = false;

		bool valid() const { return handle && !memoryAsset; }
	};

}