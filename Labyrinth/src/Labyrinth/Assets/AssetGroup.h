#pragma once

#include "IAsset.h"

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

namespace Labyrinth {

    enum class StorageType { Pool = 0, Map };

    template<typename T>
    class AssetGroup : public IAsset
    {
    public:
        struct AssetGroupItem
        {
            std::string id;
            Ref<T> asset;

            AssetGroupItem(const std::string& _id, Ref<T> data) : id(_id), asset(data) {}

            operator Ref<T>() { return asset; }
        };

    public:
        using AssetType = T;
        using AssetRef = Ref<T>;
        using GroupRef = Ref<AssetGroup>;

        using GroupPool = std::vector<AssetGroupItem>;
        using GroupMap = std::unordered_map<std::string, AssetRef>;
        using GroupType = std::variant<GroupPool, GroupMap>;

        using PoolIterator = typename std::vector<AssetGroupItem>::iterator;

    public:
        AssetGroup() : mAssets(GroupPool()) {}
        AssetGroup(StorageType storageType)
        {
            switch (storageType)
            {
            case StorageType::Pool: mAssets = GroupPool(); break;
            case StorageType::Map:  mAssets = GroupMap(); break;
            default:                mAssets = GroupPool(); break;
            }
        }

        template<typename... Args>
        AssetRef add(const std::string& id, Args... args)
        {
            AssetRef newAsset = T::Create(std::forward<Args>(args)...);
            std::visit([&id, &newAsset](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    arg.emplace_back(id, newAsset);
                else if constexpr (std::is_same_v<T, GroupMap>)
                    arg.emplace(id, newAsset);
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return newAsset;
        }
        AssetRef add(const std::string& id, const AssetRef& newAsset)
        {
            std::visit([&id, &newAsset](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    arg.emplace_back(id, newAsset);
                else if constexpr (std::is_same_v<T, GroupMap>)
                    arg.emplace(id, newAsset);
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return newAsset;
        }

        bool exists(const std::string& id)
        {
            bool result = false;
            std::visit([&id, &result](auto& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, GroupPool>)
                        result = std::find_if(arg.begin(), arg.end(), [&id](const AssetGroupItem& item) { return id == item.id; }) != arg.end();
                    else if constexpr (std::is_same_v<T, GroupMap>)
                        result = arg.count(id) != 0;
                    else
                        LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
                }, mAssets);

            return result;
        }

        void clear() 
        {
            std::visit([&id, &newAsset](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    arg.clear();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    arg.clear();
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);
        }

        AssetRef operator[](size_t index)
        {
            AssetRef result = nullptr;
            std::visit([&result, index](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                {
                    result = arg[index];
                }
                else if constexpr (std::is_same_v<T, GroupMap>)
                    LAB_STATIC_ASSERT(false, "Index operator is not valid for map!");
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return result;
        }
        AssetRef operator[](const std::string& id)
        {
            AssetRef asset = nullptr;
            std::visit([&id, &asset](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                {
                    auto it = std::find_if(arg.begin(), arg.end(), [&id](const AssetGroupItem& item) { return id == item.id; });
                    if (it != arg.end())
                        asset = it->asset;
                }
                else if constexpr (std::is_same_v<T, GroupMap>)
                    asset = arg[id];
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return asset;
        }
        AssetRef get(const std::string& id)
        {
            AssetRef asset = nullptr;
            std::visit([&id, &asset](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                {
                    auto it = std::find_if(arg.begin(), arg.end(), [&id](const AssetGroupItem& item) { return id == item.id; });
                    if (it != arg.end())
                        asset = it->asset;
                }
                else if constexpr (std::is_same_v<T, GroupMap>)
                    asset = arg[id];
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return asset;
        }

        template<typename... Args>
        AssetRef addOrGet(const std::string& id, Args... args)
        {
            if (exists(id))
                return get(id);
            else
                return add(id, std::forward<Args>(args)...);
        }

        size_t count(const std::string& id) const
        {
            size_t count = 0;;
            std::visit([&id, &count](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                {
                    auto it = std::find_if(arg.begin(), arg.end(), [&id, &count](const AssetGroupItem& item) { return id == item.id; });
                    if (it != arg.end())
                        count = 1;
                }
                else if constexpr (std::is_same_v<T, GroupMap>)
                    count = arg.count();
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return count;
        }

        constexpr size_t size() 
        {
            size_t size = 0;
            std::visit([&size](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    size = arg.size();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    size = arg.size();
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return size;
        }

        PoolIterator begin()
        {
            PoolIterator it = PoolIterator();
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    it = arg.begin();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    LAB_CORE_ERROR("Map storage can not be accessed via iterator");
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return it;
        }
        const PoolIterator begin() const
        {
            const PoolIterator it = PoolIterator();
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    it = arg.begin();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    LAB_CORE_ERROR("Map storage can not be accessed via iterator");
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return it;
        }
        PoolIterator end()
        {
            PoolIterator it;
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    it = arg.end();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    LAB_CORE_ERROR("Map storage can not be accessed via iterator");
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return it;
        }
        const PoolIterator end() const
        {
            const PoolIterator it;
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, GroupPool>)
                    it = arg.end();
                else if constexpr (std::is_same_v<T, GroupMap>)
                    LAB_CORE_ERROR("Map storage can not be accessed via iterator");
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return it;
        }

        static GroupRef Create()
        {
            GroupRef newGroup = CreateRef<AssetGroup>();
            return newGroup;
        }
        static GroupRef Create(StorageType storageType)
        {
            GroupRef newGroup = CreateRef<AssetGroup>(storageType);
            return newGroup;
        }

    private:
        GroupType mAssets;
    };

    using Tex2DGroup = AssetGroup<Texture2D>;
    using SubTex2DGroup = AssetGroup<SubTexture2D>;
    using Tex2DSheetGroup = AssetGroup<Texture2DSheet>;
}
