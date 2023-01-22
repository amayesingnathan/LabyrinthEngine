#pragma once

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Core/UUID.h>

#include "AssetTypes.h"

namespace Laby {

	using AssetHandle = UUID;

#define ASSET_STATIC_TYPE(type)	static AssetType GetStaticType() { return type; }\
								virtual AssetType getAssetType() const override { return GetStaticType(); }

	class Asset : public virtual RefCounted
	{

	public:
		AssetHandle handle;
		u16 flags = (u16)AssetFlag::None;

		virtual ~Asset() {}

		static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType getAssetType() const { return GetStaticType(); };

		bool valid() const { return ((flags & (u16)AssetFlag::Missing) | (flags & (u16)AssetFlag::Invalid)) == 0; }

		bool isFlagSet(AssetFlag flag) const { return (u16)flag & flags; }
		void setFlag(AssetFlag flag, bool value = true)
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
	concept IsAsset = DerivedFrom<Asset, T>;
}