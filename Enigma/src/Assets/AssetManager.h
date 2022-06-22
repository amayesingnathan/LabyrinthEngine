#pragma once

#include <Labyrinth.h>

namespace Labyrinth {

    template<typename T>
    class AssetGroup
    {
    public:
        using AssetType = T;
        using AssetRef = Ref<T>;
        using Group = std::vector<AssetRef>;
        using GroupRef = Ref<AssetGroup>;

        static GroupRef Create()
        {
            GroupRef newGroup = CreateRef<AssetGroup>();
            return newGroup;
        }

        template<typename... Args>
        AssetRef add(Args... args)
        {
            AssetRef newAsset = T::Create(std::forward<Args>(args)...);
            mAssets.push_back(newAsset);
            return newAsset;
        }

        void clear() { mAssets.clear(); }

        AssetRef operator[](size_t index) { return mAssets[index]; }

        constexpr size_t size() { return mAssets.size(); }

        auto begin() { return mAssets.begin(); }
        const auto begin() const { return mAssets.begin(); }
        auto end() { return mAssets.end(); }
        const auto end() const { return mAssets.end(); }

    private:
        std::vector<AssetRef> mAssets;
    };

    class AssetManager
    {
    private:
        using AssetCache = std::unordered_map<std::string, Ref<void>>;

        static AssetCache& GetAssets()
        {
            static AssetCache mAssetCache;
            return mAssetCache;
        }

    public:
        template<typename T>
        static Ref<T> Create(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<T> newAsset = T::Create();
            assets[id] = newAsset;

            return newAsset;
        }

        template<typename T, typename... Args>
        static Ref<T> Create(const std::string& id, Args... args)
        {
            AssetCache& assets = GetAssets();
            LAB_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            Ref<T> newAsset = T::Create(std::forward<Args>(args)...);
            assets[id] = newAsset;

            return newAsset;
        }

        template<typename T>
        static Ref<T> Get(const std::string& id)
        {
            AssetCache& assets = GetAssets();
            LAB_ASSERT(assets.count(id) != 0, "Asset doesn't exist in manager!");

            return CastRef<T>(assets[id]);
        }

        template<typename T>
        static void Add(const std::string& id, const Ref<T>& newAsset)
        {
            AssetCache& assets = GetAssets();
            LAB_ASSERT(assets.count(id) == 0, "Asset already exists in manager!");

            assets[id] = newAsset;
        }
    };
}