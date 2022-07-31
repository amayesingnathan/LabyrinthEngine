#pragma once

#include "AssetMetadata.h"

namespace Labyrinth {

	class AssetSerializer
	{
	public:
		virtual void serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
	};

	class TextureSerializer : public AssetSerializer
	{
	public:
		virtual void serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class SubTextureSerializer : public AssetSerializer
	{
	public:
		virtual void serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class TextureSheetSerializer : public AssetSerializer
	{
	public:
		virtual void serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class SceneAssetSerializer : public AssetSerializer
	{
	public:
		virtual void serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

}
