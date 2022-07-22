#pragma once

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/Core/UUID.h>

#include <filesystem>

namespace Labyrinth {

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture,
		Group
	};

	using AssetHandle = UUID;

	struct AssetMetaData
	{
		AssetHandle handle;
		std::filesystem::path filepath;

		bool operator==(const AssetMetaData& other) const { return handle == other.handle; }
		bool operator==(const AssetHandle& other) const { return handle == other; }
	};

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

#define ASSET_STATIC_TYPE(type)	static AssetType GetStaticType() { return type; }\
								virtual AssetType GetAssetType() const override { return GetStaticType(); }

	class Asset : public RefCounted
	{
	public:
		virtual ~Asset() {}

		virtual AssetType GetAssetType() const { return AssetType::None; };

	private:
		AssetHandle handle;

		static Ref<Asset> Create() { return Ref<Asset>::Create(); }

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