#include "Lpch.h"
#include "AssetImporter.h"
#include "AssetManager.h"

namespace Labyrinth {

	void AssetImporter::Init()
	{
		sSerialisers[AssetType::Scene] = CreateSingle<SceneAssetSerializer>();
		sSerialisers[AssetType::Texture] = CreateSingle<TextureSerializer>();
		sSerialisers[AssetType::SubTexture] = CreateSingle<TextureSerializer>();
	}

	void AssetImporter::Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (sSerialisers.count(metadata.type) == 0)
		{
			LAB_CORE_WARN("There's currently no importer for assets of type {0}", metadata.filepath.stem().string());
			return;
		}

		sSerialisers[asset->getAssetType()]->serialize(metadata, asset);
	}

	void AssetImporter::Serialise(const Ref<Asset>& asset)
	{
		const AssetMetadata& metadata = AssetManager::GetMetadata(asset->handle);
		Serialise(metadata, asset);
	}

	bool AssetImporter::Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset)
	{
		if (sSerialisers.count(metadata.type) == 0)
		{
			LAB_CORE_WARN("There's currently no importer for assets of type {0}", metadata.filepath.stem().string());
			return false;
		}

		return sSerialisers[metadata.type]->deserialise(metadata, asset);
	}
}