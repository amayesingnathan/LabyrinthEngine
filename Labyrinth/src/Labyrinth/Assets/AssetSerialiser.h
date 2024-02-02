#pragma once

#include "AssetMetadata.h"

namespace Laby {

	class AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
	};

	class TextureSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class SubTextureSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class TextureSheetSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class SceneAssetSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class TilemapSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class AnimationSerialiser : public AssetSerialiser
	{
	public:
		virtual void Serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool Deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

}
