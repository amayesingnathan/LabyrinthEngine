#pragma once

#include "AssetSerialiser.h"

namespace Labyrinth {

	class AssetImporter
	{
	public:
		static void Init();
		static void Shutdown();
		static void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset);
		static void Serialise(const Ref<Asset>& asset);
		static bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset);

	private:
		inline static std::unordered_map<AssetType, Single<AssetSerializer>> sSerialisers;
	};

}
