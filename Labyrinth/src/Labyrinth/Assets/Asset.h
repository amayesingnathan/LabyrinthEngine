#pragma once

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Core/UUID.h>

#include "AssetTypes.h"

namespace Laby {

	using AssetHandle = UUID;

#define ASSET_METADATA(type, directory)	static constexpr std::string_view GetAssetDirectory() { return directory; }\
										static constexpr AssetType GetStaticType() { return type; }\
										virtual constexpr AssetType GetAssetType() const override { return GetStaticType(); }

	class Asset : public virtual RefCounted
	{
	public:
		AssetHandle handle;
		u16 flags = (u16)AssetFlag::None;

		virtual ~Asset() {}

		static constexpr std::string_view GetAssetDirectory() { return ""; }
		static constexpr AssetType GetStaticType() { return AssetType::None; }
		virtual constexpr AssetType GetAssetType() const { return GetStaticType(); };

		bool Valid() const { return ((flags & (u16)AssetFlag::Missing) | (flags & (u16)AssetFlag::Invalid)) == 0; }

		bool IsFlagSet(AssetFlag flag) const { return (u16)flag & flags; }
		void SetFlag(AssetFlag flag, bool value = true)
		{
			if (value)
				flags |= (u16)flag;
			else
				flags &= ~(u16)flag;
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

	template<typename T>
	concept IsAsset = DerivedFrom<Asset, T> && requires { T::GetAssetDirectory(); T::GetStaticType(); };
}