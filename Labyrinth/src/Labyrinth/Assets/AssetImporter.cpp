#include "Lpch.h"
#include "AssetImporter.h"
#include "AssetManager.h"

#include <Labyrinth/Tools/EnumUtils.h>

namespace Laby {

	void AssetImporter::Init()
	{
		sSerialisers[AssetType::Scene] = MakeSingle<SceneAssetSerialiser>();
		sSerialisers[AssetType::Texture] = MakeSingle<TextureSerialiser>();
		sSerialisers[AssetType::TextureSheet] = MakeSingle<TextureSheetSerialiser>();
		sSerialisers[AssetType::SubTexture] = MakeSingle<SubTextureSerialiser>();
		sSerialisers[AssetType::Tilemap] = MakeSingle<TilemapSerialiser>();
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
		if (!sSerialisers.contains(metadata.type))
		{
			LAB_CORE_WARN("There's currently no importer for assets of type {0}", Enum::ToString(metadata.type));
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
		if (!sSerialisers.contains(metadata.type))
		{
			LAB_CORE_WARN("There's currently no importer for assets of type {0}", Enum::ToString(metadata.type));
			return false;
		}

		return sSerialisers[metadata.type]->deserialise(metadata, asset);
	}
}