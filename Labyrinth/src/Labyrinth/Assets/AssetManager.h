#pragma once

#include "Asset.h"
#include "AssetGroup.h"

#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include <unordered_map>
#include <unordered_set>

namespace Labyrinth {

    class AssetManager
    {
    private:
        AssetManager() = default;
        AssetManager(const AssetManager&) = delete;
        void operator=(const AssetManager&) = delete;

    private:
        struct AssetCacheItem
        {
            Ref<Asset> asset;
            AssetMetaData metadata;
        };

    private:
        using AssetCache = std::unordered_map<AssetHandle, AssetCacheItem>;
        AssetCache mCache;

    private:
        static AssetManager& GetSingleton()
        {
            static AssetManager mAssetManager;
            return mAssetManager;
        }

    public:
        /// <summary>
        /// The main access function for the whole cache of assets.
        /// </summary>
        /// <returns>Unordered map to Asset refs with string keys</returns>
        static AssetCache& GetAssets() { return GetSingleton().mCache; }

        /// <summary>
        /// Creates a new asset of type AssetType in the cache against the key 'id', using the supplied constructor arguments.
        /// Stored as a Ref to an Asset, retrieve using AssetManager::Get(id) with the same AssetType template.
        /// </summary>
        /// <param name="...args">AssetType constructor arguments</param>
        /// <returns>A Ref to the newly created asset</returns>
        template<typename AssetType, typename... Args>
        static Ref<AssetType> CreateAsset(const std::string& filename, const std::string& directory, Args&&... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<Asset, AssetType>());

            AssetCache& assets = GetAssets();

            AssetMetaData metadata;
            metadata.filepath = directory + filename;

            Ref<AssetType> newAsset = Ref<AssetType>::Create(std::forward<Args>(args)...);
            newAsset->handle = metadata.handle;

            AssetCacheItem item = { newAsset, metadata };
            assets[metadata.handle] = item;
            return newAsset;
        }

        /// <summary>
        /// Delete asset from cache with key 'id'. Asserts if it does not exist.
        /// Any instances of the asset in use will keep the asset loaded, they will just no longer be kept alive by the asset manager.
        /// </summary>
        static void DeleteAsset(const AssetHandle& handle)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(handle) != 0, "Asset does not exist in manager!");

            assets.erase(handle);
        }

        /// <summary>
        /// Get asset from cache with key 'id'.
        /// </summary>
        /// <typeparam name="AssetType">The type of asset to get from cache</typeparam>
        template<typename AssetType>
        static Ref<AssetType> GetAsset(const AssetHandle& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset doesn't exist in manager!");

            return assets[id].asset;
        }

        static AssetHandle GetAssetHandleFromPath(const std::filesystem::path& path)
        {
            return AssetHandle();
        }

        /// <summary>
        /// Returns check if asset with key 'id' exists in cache.
        /// </summary>
        static bool Exists(const AssetHandle& id) { return GetAssets().count(id) != 0; }

    private:
        template<typename AssetType, typename... Args>
        static Ref<AssetType> CreateAssetWithHandle(const AssetHandle& handle, const std::string& filename, const std::string& directory, Args&&... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<Asset, AssetType>());

            AssetCache& assets = GetAssets();

            AssetMetaData metadata;
            metadata.filepath = directory + filename;
            metadata.handle = handle;

            Ref<AssetType> newAsset = AssetType::Create(std::forward<Args>(args)...);
            newAsset->handle = metadata.handle;

            assets[metadata.handle] = newAsset;
            return newAsset;
        }
    };
}