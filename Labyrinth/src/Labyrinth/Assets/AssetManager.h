#pragma once

#include "AssetExtensions.h"
#include "AssetRegistry.h"
#include "AssetImporter.h"

namespace Laby {

	class AssetManager
	{
	public:
		using AssetCache = std::unordered_map<AssetHandle, Ref<Asset>>;

    public:
        static void Init();
        static void Shutdown();

        static const AssetMetadata& GetMetadata(AssetHandle handle);
        static const AssetMetadata& GetMetadata(const fs::path filepath);

        static AssetHandle GetHandleFromPath(const fs::path& filepath);

        static fs::path GetRelativePath(const fs::path& filepath);
        static fs::path GetFileSystemPath(const AssetMetadata& metadata) { return sAssetDirPath / metadata.filepath; }
        static fs::path GetFileSystemPath(AssetHandle handle) { return GetFileSystemPath(GetMetadataInternal(handle)); }
        static std::string GetFileSystemPathString(const AssetMetadata& metadata) { return GetFileSystemPath(metadata).string(); }

        static bool IsAssetHandleValid(AssetHandle assetHandle) { return IsMemoryAsset(assetHandle) || GetMetadata(assetHandle).valid(); }

        static bool IsMemoryAsset(AssetHandle handle) { return sMemoryAssets.count(handle) != 0; }

        static AssetType GetAssetTypeFromExtension(const std::string& extension);
        static AssetType GetAssetTypeFromPath(const fs::path& path);
        static AssetType GetAssetTypeFromHandle(AssetHandle handle);

        static bool IsExtensionValid(const fs::path& extension, AssetType type);

        static void ReloadAssets();
        static AssetHandle ImportAsset(const fs::path& filepath);
        static bool ReloadData(AssetHandle assetHandle);

        static void DestroyAsset(AssetHandle assetHandle);

    private:
        static void LoadRegistry();
        static void ProcessDirectory(const fs::path& directoryPath);
        static void SaveRegistry();

        static AssetMetadata& GetMetadataInternal(AssetHandle handle);

        template<IsAsset T>
        static Ref<T> ShallowCopyCreateAsset(std::string_view filename, AssetHandle existingAsset)
        {
            fs::path directoryPath = T::GetAssetDirectory();

            if (!fs::exists(sAssetDirPath / directoryPath))
                FileUtils::CreateDir(sAssetDirPath / directoryPath);

            AssetType type = T::GetStaticType();
            fs::path filenameExt = std::format("{}{}", filename, sAssetCreationMap[type]);

            AssetMetadata metadata;
            metadata.handle = AssetHandle();
            if (directoryPath.empty() || directoryPath == ".")
                metadata.filepath = filenameExt;
            else
                metadata.filepath = AssetManager::GetRelativePath(directoryPath / filenameExt);
            metadata.dataLoaded = true;
            metadata.type = type;

            if (AssetManager::FileExists(metadata))
            {
                bool foundAvailableFileName = false;
                int current = 1;

                while (!foundAvailableFileName)
                {
                    fs::path nextFilePath = directoryPath / metadata.filepath.stem();

                    if (current < 10)
                        nextFilePath += std::format(" (0{})", current);
                    else
                        nextFilePath += std::format(" ({})", current);
                    nextFilePath += metadata.filepath.extension();

                    if (!fs::exists(nextFilePath))
                    {
                        foundAvailableFileName = true;
                        metadata.filepath = AssetManager::GetRelativePath(nextFilePath);
                        break;
                    }

                    current++;
                }
            }

            sAssetRegistry[metadata.handle] = metadata;

            Ref<T> asset = GetAsset<T>(existingAsset);
            asset->handle = metadata.handle;
            sLoadedAssets[asset->handle] = asset;
            AssetImporter::Serialise(metadata, asset);

            return asset;
        }

    public:
        template<IsAsset T, typename... Args>
        static Ref<T> CreateNewAsset(std::string_view filename, Args&&... args)
        {
            fs::path directoryPath = T::GetAssetDirectory();

            if (!fs::exists(sAssetDirPath / directoryPath))
                FileUtils::CreateDir(sAssetDirPath / directoryPath);

            AssetType type = T::GetStaticType();
            fs::path filenameExt = std::format("{}{}", filename, sAssetCreationMap[type]);

            AssetMetadata metadata;
            metadata.handle = AssetHandle();
            if (directoryPath.empty() || directoryPath == ".")
                metadata.filepath = filenameExt;
            else
                metadata.filepath = AssetManager::GetRelativePath(directoryPath / filenameExt);
            metadata.dataLoaded = true;
            metadata.type = type;

            if (AssetManager::FileExists(metadata))
            {
                bool foundAvailableFileName = false;
                int current = 1;

                while (!foundAvailableFileName)
                {
                    fs::path nextFilePath = directoryPath  / metadata.filepath.stem();

                    if (current < 10)
                        nextFilePath += std::format(" (0{})", current);
                    else
                        nextFilePath += std::format(" ({})", current);
                    nextFilePath += metadata.filepath.extension();

                    if (!fs::exists(nextFilePath))
                    {
                        foundAvailableFileName = true;
                        metadata.filepath = AssetManager::GetRelativePath(nextFilePath);
                        break;
                    }

                    current++;
                }
            }

            sAssetRegistry[metadata.handle] = metadata;

            Ref<T> asset = Ref<T>::Create(std::forward<Args>(args)...);
            asset->handle = metadata.handle;
            sLoadedAssets[asset->handle] = asset;
            AssetImporter::Serialise(metadata, asset);

            return asset;
        }

        template<IsAsset T>
        static Ref<T> GetAsset(AssetHandle assetHandle)
        {
            if (IsMemoryAsset(assetHandle))
                return sMemoryAssets[assetHandle].to<T>();

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

            return asset.to<T>();
        }

        template<IsAsset T>
        static Ref<T> GetAsset(const fs::path& filepath)
        {
            return GetAsset<T>(GetAssetHandleFromPath(filepath));
        }

        template<IsAsset T>
        static Ref<T> SaveMemoryOnlyAsset(std::string_view filename, AssetHandle handle)
        {
            if (!IsMemoryAsset(handle))
            {
                LAB_CORE_ERROR("Tried to save asset that was not memory only!");
                return nullptr;
            }

            return ShallowCopyCreateAsset<T>(filename, handle);
        }

        static bool FileExists(AssetMetadata& metadata)
        {
            return fs::exists(sAssetRegPath / metadata.filepath);
        }

        static AssetHandle GetAssetHandleFromPath(const fs::path& path)
        {
            return GetMetadata(path).handle;
        }

        static const AssetCache& GetLoadedAssets() { return sLoadedAssets; }
        static const AssetRegistry& GetAssetRegistry() { return sAssetRegistry; }
        static const AssetCache& GetMemoryOnlyAssets() { return sMemoryAssets; }

        template<IsAsset T>
        static auto GetAssetsWithType()
        {
            return sAssetRegistry |
                std::views::filter([](const auto& entry) { return entry.second.type == T::GetStaticType(); }) |
                std::views::transform([](const auto& entry) { return AssetManager::GetAsset<T>(entry.first); }) |
                std::views::filter([](const Ref<T> asset) { return asset != nullptr; });
        }

        template<IsAsset T>
        static auto GetAssetHandlesWithType()
        {
            return sAssetRegistry |
                std::views::filter([](const auto& entry) { return entry.second.type == T::GetStaticType(); }) |
                std::views::keys;
        }

        template<IsAsset T, typename... Args>
        static AssetHandle CreateMemoryOnlyAsset(Args&&... args)
        {
            Ref<T> asset = Ref<T>::Create(std::forward<Args>(args)...);
            asset->handle = AssetHandle();

            sMemoryAssets[asset->handle] = asset;
            return asset->handle;
        }

    private:
        template<IsAsset T, typename... Args>
        static Ref<T> CreateAssetWithHandle(const AssetHandle& handle, const std::string& filename, const std::string& directory, Args&&... args)
        {
            AssetMetadata metadata;
            metadata.filepath = directory + filename;
            metadata.handle = handle;

            Ref<T> newAsset = Ref<T>::Create(std::forward<Args>(args)...);
            newAsset->handle = metadata.handle;

            sLoadedAssets[metadata.handle] = newAsset;
            return newAsset;
        }

    private:
        inline static AssetCache sLoadedAssets;
        inline static AssetCache sMemoryAssets;
        inline static AssetRegistry sAssetRegistry;
        inline static fs::path sAssetDirPath, sAssetRegPath;

        friend class Asset;
	};

}