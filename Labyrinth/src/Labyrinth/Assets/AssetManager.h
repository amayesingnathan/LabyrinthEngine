#pragma once

#include "IAsset.h"

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

        friend class AssetPanel;

    private:
        static AssetManager& Get()
        {
            static AssetManager mAssetManager;
            return mAssetManager;
        }

        static AssetCache& GetAssets() { return Get().mCache;  }

    public:
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
    };
}