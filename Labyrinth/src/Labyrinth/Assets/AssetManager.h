#pragma once

#include "AssetRegistry.h"
#include "AssetGroup.h"
#include "AssetImporter.h"

#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Tools/PlatformUtils.h"
#include "Labyrinth/Tools/Profiling.h"

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

        static std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
        static std::filesystem::path GetFileSystemPath(const AssetMetadata& metadata) { return sAssetDirPath / metadata.filepath; }
        static std::string GetFileSystemPathString(const AssetMetadata& metadata) { return GetFileSystemPath(metadata).string(); }

        static bool IsAssetHandleValid(AssetHandle assetHandle) { return IsMemoryAsset(assetHandle) || GetMetadata(assetHandle).valid(); }

        static bool IsMemoryAsset(AssetHandle handle) { return sMemoryAssets.count(handle) != 0; }

        static AssetType GetAssetTypeFromExtension(const std::string& extension);
        static AssetType GetAssetTypeFromPath(const std::filesystem::path& path);

        static AssetHandle ImportAsset(const std::filesystem::path& filepath);
        static bool ReloadData(AssetHandle assetHandle);

    private:
        static void LoadRegistry();
        static void ProcessDirectory(const std::filesystem::path& directoryPath);
        static void ReloadAssets();
        static void SaveRegistry();

        static AssetMetadata& GetMetadataInternal(AssetHandle handle);

    public:
        template<typename AssetType, typename... Args>
        static Ref<AssetType> CreateNewAsset(const std::string& filename, const std::string& directoryPath, Args&&... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<Asset, AssetType>());

            AssetMetadata metadata;
            metadata.handle = AssetHandle();
            if (directoryPath.empty() || directoryPath == ".")
                metadata.filepath = filename;
            else
                metadata.filepath = AssetManager::GetRelativePath(directoryPath + "/" + filename);
            metadata.dataLoaded = true;
            metadata.type = AssetType::GetStaticType();

            if (AssetManager::FileExists(metadata))
            {
                bool foundAvailableFileName = false;
                int current = 1;

                while (!foundAvailableFileName)
                {
                    std::string nextFilePath = directoryPath + "/" + metadata.filepath.stem().string();

                    if (current < 10)
                        nextFilePath += " (0" + std::to_string(current) + ")";
                    else
                        nextFilePath += " (" + std::to_string(current) + ")";
                    nextFilePath += metadata.filepath.extension().string();

                    if (!std::filesystem::exists(nextFilePath))
                    {
                        foundAvailableFileName = true;
                        metadata.filepath = AssetManager::GetRelativePath(nextFilePath);
                        break;
                    }

                    current++;
                }
            }

            sAssetRegistry[metadata.handle] = metadata;

            SaveRegistry();

            Ref<AssetType> asset = Ref<AssetType>::Create(std::forward<Args>(args)...);
            asset->handle = metadata.handle;
            sLoadedAssets[asset->handle] = asset;
            AssetImporter::Serialise(metadata, asset);

            return asset;
        }

        template<typename AssetType>
        static Ref<AssetType> GetAsset(const AssetHandle& assetHandle)
        {
            LAB_PROFILE_FUNCTION();

            if (IsMemoryAsset(assetHandle))
                return sMemoryAssets[assetHandle].to<AssetType>();

            auto& metadata = GetMetadataInternal(assetHandle);
            if (!metadata.valid())
                return nullptr;

            Ref<Asset> asset = nullptr;
            if (!metadata.dataLoaded)
            {
                metadata.dataLoaded = AssetImporter::Deserialise(metadata, asset);
                if (!metadata.dataLoaded)
                    return nullptr;

                sLoadedAssets[assetHandle] = asset;
            }
            else asset = sLoadedAssets[assetHandle];

            return asset.to<AssetType>();
        }

        template<typename T>
        static Ref<T> GetAsset(const std::string& filepath)
        {
            return GetAsset<T>(GetAssetHandleFromFilePath(filepath));
        }

        static bool FileExists(AssetMetadata& metadata)
        {
            return std::filesystem::exists(sAssetRegPath / metadata.filepath);
        }

        static AssetHandle GetAssetHandleFromPath(const std::filesystem::path& path)
        {
            return AssetHandle();
        }

        static const AssetCache& GetLoadedAssets() { return sLoadedAssets; }
        static const AssetRegistry& GetAssetRegistry() { return sAssetRegistry; }
        static const AssetCache& GetMemoryOnlyAssets() { return sMemoryAssets; }

        template<typename AssetType, typename... Args>
        static AssetHandle CreateMemoryOnlyAsset(Args&&... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<Asset, AssetType>(), "CreateMemoryOnlyAsset only works for types derived from Asset");

            Ref<AssetType> asset = Ref<AssetType>::Create(std::forward<Args>(args)...);
            asset->handle = AssetHandle();

            sMemoryAssets[asset->handle] = asset;
            return asset->handle;
        }

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
        inline static AssetCache sLoadedAssets;
        inline static AssetCache sMemoryAssets;
        inline static AssetRegistry sAssetRegistry;
        inline static std::filesystem::path sAssetDirPath, sAssetRegPath;
    };
}