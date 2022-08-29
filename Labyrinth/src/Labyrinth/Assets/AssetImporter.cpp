#include "Lpch.h"
#include "AssetImporter.h"
#include "AssetManager.h"

namespace Labyrinth {

	void AssetImporter::Init()
	{
		sSerialisers[AssetType::Scene] = CreateSingle<SceneAssetSerialiser>();
		sSerialisers[AssetType::Texture] = CreateSingle<TextureSerialiser>();
		sSerialisers[AssetType::TextureSheet] = CreateSingle<TextureSheetSerialiser>();
		sSerialisers[AssetType::SubTexture] = CreateSingle<SubTextureSerialiser>();
		sSerialisers[AssetType::Tilemap] = CreateSingle<TilemapSerialiser>();
	}

	void AssetImporter::Shutdown()
	{
		sSerialisers[AssetType::Scene].reset();
		sSerialisers[AssetType::Texture].reset();
		sSerialisers[AssetType::TextureSheet].reset();
		sSerialisers[AssetType::SubTexture].reset();
		sSerialisers[AssetType::Tilemap].reset();
	}

	void AssetImporter::Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (sSerialisers.count(metadata.type) == 0)
		{
			LAB_CORE_WARN("There's currently no importer for assets of type {0}", metadata.filepath.stem().string());
			return;
		}

		sSerialisers[asset->getAssetType()]->serialise(metadata, asset);
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