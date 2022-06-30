#pragma once

#include "IAsset.h"
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
        using AssetCache = std::unordered_map<std::string, Ref<IAsset>>;
        AssetCache mCache;

    private:
        static AssetManager& GetSingleton()
        {
            static AssetManager mAssetManager;
            return mAssetManager;
        }

        template<typename... AssetType>
        static size_t GetGroupRefCount(const Ref<IAsset>& asset)
        {
            size_t result = 0;
            ([&result, &asset]()
            {
                // Check each possible asset type to see if this is an AssetGroup of each type.
                const Ref<AssetGroup<AssetType>>& isGroup = CastRefToRelative<AssetGroup<AssetType>>(asset);
                if (isGroup)
                    result += isGroup->ref_count();
            }(), ...);

            return result;
        }
        template<typename... AssetType>
        static size_t GetGroupRefCount(AssetTypeGroup<AssetType...>, const Ref<IAsset>& asset)
        {
            return GetGroupRefCount<AssetType...>(asset);
        }
        static size_t GetSubTexRefCount(const Ref<IAsset>& asset)
        {
            size_t count = 0;

            // If this is not a sheet then no subtextures to count.
            Ref<Texture2DSheet> isSheet = CastRefToRelative<Texture2DSheet>(asset);
            if (!isSheet) return count;

            for (const auto& [name, tex] : isSheet->getSubTexList())
                count += (tex.use_count() - 1);

            return count;
        }

        template<typename... AssetType>
        static bool IsGroup(const Ref<IAsset>& asset)
        {
            bool result = false;
            ([&result, &asset]()
            {
                // Check each possible asset type to see if this is an AssetGroup of each type.
                Ref<AssetGroup<AssetType>> isGroup = CastRefToRelative<AssetGroup<AssetType>>(asset);
                if (isGroup)
                    result = true;
            }(), ...);

            return result;
        }
        template<typename... AssetType>
        static bool IsGroup(AssetTypeGroup<AssetType...>, const Ref<IAsset>& asset)
        {
            return IsGroup<AssetType...>(asset);
        }

        template<typename... AssetGroupType>
        static AssetGroupVariant GetGroup(const Ref<IAsset>& asset)
        {
            AssetGroupVariant result;
            ([&result, &asset]()
            {
                // Check each possible asset type to see if this is an AssetGroup of each type.
                Ref<AssetGroupType> isGroup = CastRefToRelative<AssetGroupType>(asset);
                if (isGroup)
                    result = isGroup;
            }(), ...);

            return result;
        }
        template<typename... AssetGroupType>
        static AssetGroupVariant GetGroup(AssetGroups<AssetGroupType...>, const Ref<IAsset>& asset)
        {
            return GetGroup<AssetGroupType...>(asset);
        }

    public:
        static AssetCache& GetAssets() { return GetSingleton().mCache; }

        template<typename T>
        static Ref<T> Create(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<T> newAsset = T::Create();
            assets[id] = CastRefToRelative<IAsset>(newAsset);

            return newAsset;
        }

        template<typename T, typename... Args>
        static Ref<T> Create(const std::string& id, Args... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<IAsset, T>());

            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<T> newAsset = T::Create(std::forward<Args>(args)...);
            assets[id] = CastRefToRelative<IAsset>(newAsset);

            return newAsset;
        }

        template<typename T>
        static void Add(const std::string& id, const Ref<T>& newAsset)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            assets[id] = CastRefToRelative<IAsset>(newAsset);
        }

        static void Delete(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset does not exist in manager!");

            assets.erase(id);
        }

        template<typename T>
        static Ref<T> Get(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset doesn't exist in manager!");

            return CastRefToRelative<T>(assets[id]);
        }

        template<typename T, typename... Args>
        static Ref<T> GetOrCreate(const std::string& id, Args... args)
        {
            AssetCache& assets = GetAssets();
            if (assets.count(id) == 0)
                return Create<T>(id, std::forward<Args>(args)...);
            else
                return Get<T>(id);
        }

        static bool Exists(const std::string& id) { return GetAssets().count(id) != 0; }

        static size_t GetRefCount(const Ref<IAsset>& asset)
        {
            size_t count = asset.use_count() - 1;
            count += GetGroupRefCount(AllAssetTypes{}, asset); // Will add zero if not a group
            count += GetSubTexRefCount(asset);
            return count;
        }

        static bool IsGroup(const Ref<IAsset>& asset)
        {
            return IsGroup(AllAssetTypes{}, asset);
        }

        static AssetGroupVariant GetGroup(const Ref<IAsset>& asset)
        {
            return GetGroup(AllAssetGroups{}, asset);
        }
    };
}