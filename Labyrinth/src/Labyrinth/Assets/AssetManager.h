#pragma once

#include "AssetRegistry.h"
#include "AssetGroup.h"

#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Tools/PlatformUtils.h"

#include <unordered_map>

namespace Labyrinth {

    class AssetManager
    {
    public:
        using AssetCache = std::unordered_map<AssetHandle, Ref<Asset>>;

    public:
        static void Init();
        static void Shutdown();

        static const AssetMetadata& GetMetadata(AssetHandle handle);
        static const AssetMetadata& GetMetadata(const std::filesystem::path filepath);

        static AssetHandle GetHandleFromPath(const std::filesystem::path& filepath);

    private:
        static void LoadRegistry();
        static void SaveRegistry();

        static std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
        static std::filesystem::path GetFileSystemPath(const AssetMetadata& metadata) { return sAssetRegPath / metadata.filepath; }

    public:
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

        static const AssetCache& GetLoadedAssets() { return sLoadedAssets; }
        static const AssetRegistry& GetAssetRegistry() { return sAssetRegistry; }
        static const AssetCache& GetMemoryOnlyAssets() { return sMemoryAssets; }

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


    private:
        static AssetCache sLoadedAssets;
        static AssetCache sMemoryAssets;
        inline static AssetRegistry sAssetRegistry;
        inline static std::filesystem::path sAssetRegPath;
    };
}