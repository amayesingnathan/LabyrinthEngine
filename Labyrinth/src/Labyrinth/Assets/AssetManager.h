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
        static usize GetGroupRefCount(const Ref<IAsset>& asset)
        {
            usize result = 0;
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
        static usize GetGroupRefCount(AssetTypeGroup<AssetType...>, const Ref<IAsset>& asset)
        {
            return GetGroupRefCount<AssetType...>(asset);
        }
        static usize GetSubTexRefCount(const Ref<IAsset>& asset);

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
                if (Ref<AssetGroupType> isGroup = CastRefToRelative<AssetGroupType>(asset))
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
        /// <summary>
        /// The main access function for the whole cache of assets.
        /// </summary>
        /// <returns>Unordered map to IAsset refs with string keys</returns>
        static AssetCache& GetAssets() { return GetSingleton().mCache; }

        /// <summary>
        /// Creates a new asset of type AssetType in the cache, against the key 'id'.
        /// Stored as a Ref to an IAsset, retrieve using AssetManager::Get(id) with the same AssetType template.
        /// </summary>
        /// <typeparam name="AssetType">The type of asset to create</typeparam>
        /// <returns>A Ref to the newly created asset</returns>
        template<typename AssetType>
        static Ref<AssetType> Create(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<AssetType> newAsset = AssetType::Create();
            assets[id] = CastRefToRelative<IAsset>(newAsset);

            return newAsset;
        }

        /// <summary>
        /// Creates a new asset of type AssetType in the cache against the key 'id', using the supplied constructor arguments.
        /// Stored as a Ref to an IAsset, retrieve using AssetManager::Get(id) with the same AssetType template.
        /// </summary>
        /// <param name="...args">AssetType constructor arguments</param>
        /// <returns>A Ref to the newly created asset</returns>
        template<typename AssetType, typename... Args>
        static Ref<AssetType> Create(const std::string& id, Args&&... args)
        {
            LAB_STATIC_ASSERT(IsDerivedFrom<IAsset, AssetType>());

            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<AssetType> newAsset = AssetType::Create(std::forward<Args>(args)...);
            assets[id] = CastRefToRelative<IAsset>(newAsset);

            return newAsset;
        }

        /// <summary>
        /// Add an existing asset to the cache, against the key 'id'.
        /// Stored as a Ref to an IAsset, retrieve using AssetManager::Get(id) with the same AssetType template.
        /// </summary>
        /// <typeparam name="AssetType">The type of asset being added</typeparam>
        template<typename AssetType>
        static void Add(const std::string& id, const Ref<AssetType>& newAsset)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            assets[id] = CastRefToRelative<IAsset>(newAsset);
        }

        /// <summary>
        /// Delete asset from cache with key 'id'. Asserts if it does not exist.
        /// Any instances of the asset in use will keep the asset loaded, they will just no longer be kept alive by the asset manager.
        /// </summary>
        static void Delete(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset does not exist in manager!");

            assets.erase(id);
        }

        /// <summary>
        /// Get asset from cache with key 'id'.
        /// </summary>
        /// <returns>Ref to the base IAsset</returns>
        static Ref<IAsset> Get(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset doesn't exist in manager!");

            return CastRefToRelative<IAsset>(assets[id]);
        }

        /// <summary>
        /// Get asset from cache with key 'id'.
        /// </summary>
        /// <typeparam name="AssetType">The type of asset to get from cache</typeparam>
        template<typename AssetType>
        static Ref<AssetType> Get(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_CORE_ASSERT(assets.count(id) != 0, "Asset doesn't exist in manager!");

            return CastRefToRelative<AssetType>(assets[id]);
        }

        /// <summary>
        /// Gets or creates an asset in the cache with key 'id' and constructor arguments 'args'.
        /// Constructor arguments are discarded if the asset already exists
        /// </summary>
        /// <typeparam name="AssetType">The type of asset to create or get</typeparam>
        /// <param name="...args">AssetType constructor arguments</param>
        /// <returns>The new asset or requested asset</returns>
        template<typename AssetType, typename... Args>
        static Ref<AssetType> GetOrCreate(const std::string& id, Args&&... args)
        {
            AssetCache& assets = GetAssets();
            if (assets.count(id) == 0)
                return Create<AssetType>(id, std::forward<Args>(args)...);
            else
                return Get<AssetType>(id);
        }

        /// <summary>
        /// Get asset from cache with key 'id'. Returns an empty ref if it does not exist.
        /// </summary>
        /// <typeparam name="AssetType">The type of asset to get</typeparam>
        template<typename AssetType>
        static Ref<AssetType> GetOrNull(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            if (assets.count(id) != 0)
                return nullptr;

            return CastRefToRelative<AssetType>(assets[id]);
        }

        /// <summary>
        /// Returns check if asset with key 'id' exists in cache.
        /// </summary>
        static bool Exists(const std::string& id) { return GetAssets().count(id) != 0; }

        /// <summary>
        /// Get the number of strong references to 'asset'.
        /// </summary>
        static usize GetRefCount(const Ref<IAsset>& asset)
        {
            usize count = asset.use_count() - 1;
            count += GetGroupRefCount(AllAssetTypes{}, asset); // Will add zero if not a group
            count += GetSubTexRefCount(asset); // Will add zero if not a texture sheet
            return count;
        }
        /// <summary>
        /// Get the number of strong references to 'asset'.
        /// </summary>
        /// <returns>Return zero if 'id' does not exist in cache</returns>
        static usize GetRefCount(const std::string& id)
        {
            if (!Exists(id)) return 0;
            return GetRefCount(Get(id));
        }

        /// <summary>
        /// Checks if given asset is an AssetGroup.
        /// </summary>
        static bool IsGroup(const Ref<IAsset>& asset)
        {
            return IsGroup(AllAssetTypes{}, asset);
        }

        /// <summary>
        /// Gets a variant of all asset group types.
        /// If 'asset' is an AssetGroup then the variant contains that group.
        /// Otherwise it contains the empty struct NotGroup 
        /// </summary>
        static AssetGroupVariant GetGroup(const Ref<IAsset>& asset)
        {
            return GetGroup(AllAssetGroups{}, asset);
        }
    };
}