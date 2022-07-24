#pragma once

#include "AssetTypes.h"

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/Core/UUID.h>

#include <filesystem>

namespace Labyrinth {

	using AssetHandle = UUID;

#define ASSET_STATIC_TYPE(type)	static AssetType GetStaticType() { return type; }\
								virtual AssetType GetAssetType() const override { return GetStaticType(); }

	class Asset : public RefCounted
	{
	public:
		virtual ~Asset() {}

		virtual AssetType GetAssetType() const { return AssetType::None; };

	private:
		AssetHandle handle;

		friend class AssetManager;
	};

	class AssetSerialiser
	{
	public:
		virtual void serialise(const Ref<Asset>& asset) const = 0;
		virtual bool deserialise(const Ref<Asset>& asset) const = 0;
	};

	class Texture2D;
	class SubTexture2D;
	class Texture2DSheet;

	template<typename... AssetType>
	struct AssetTypeGroup {};
	using AllAssetTypes = AssetTypeGroup
	<
		Texture2D,
		SubTexture2D,
		Texture2DSheet
	>;

}