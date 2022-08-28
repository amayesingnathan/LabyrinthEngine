#pragma once

#include "AssetTypes.h"

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/Core/UUID.h>

#include <filesystem>

namespace Labyrinth {

	using AssetHandle = UUID;

#define ASSET_STATIC_TYPE(type)	static AssetType GetStaticType() { return type; }\
								virtual AssetType getAssetType() const override { return GetStaticType(); }

	class Asset : public RefCounted
	{
	public:
		AssetHandle handle;
		u16 flags = (u16)AssetFlag::None;

		virtual ~Asset() {}

		static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType getAssetType() const { return GetStaticType(); };

		bool valid() const { return ((flags & (u16)AssetFlag::Missing) | (flags & (u16)AssetFlag::Invalid)) == 0; }

		bool isFlagSet(AssetFlag flag) const { return (uint16_t)flag & flags; }
		void setFlag(AssetFlag flag, bool value = true)
		{
			if (value)
				flags |= (uint16_t)flag;
			else
				flags &= ~(uint16_t)flag;
		}

		virtual bool operator==(const Asset& other) const
		{
			return handle == other.handle;
		}

		virtual bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}
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