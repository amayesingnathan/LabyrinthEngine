#pragma once

#include "AssetMetadata.h"

namespace Laby {

	class AssetRegistry
	{
	public:
		AssetMetadata& operator[](const AssetHandle handle) { return mAssetRegistry[handle]; }
		const AssetMetadata& get(const AssetHandle handle) const
		{
			LAB_CORE_ASSERT(mAssetRegistry.contains(handle));
			return mAssetRegistry.at(handle);
		}

		usize size() const { return mAssetRegistry.size(); }
		bool contains(const AssetHandle handle) const { return mAssetRegistry.contains(handle); }
		usize remove(const AssetHandle handle) { return mAssetRegistry.erase(handle); }
		void clear() { mAssetRegistry.clear(); }

		auto begin() { return mAssetRegistry.begin(); }
		auto end() { return mAssetRegistry.end(); }
		auto begin() const { return mAssetRegistry.cbegin(); }
		auto end() const { return mAssetRegistry.cend(); }

	private:
		std::unordered_map<AssetHandle, AssetMetadata> mAssetRegistry;
	};

}