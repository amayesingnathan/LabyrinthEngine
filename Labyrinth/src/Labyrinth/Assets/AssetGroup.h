#pragma once

#include "IAsset.h"

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

namespace Labyrinth {

    enum class StorageType { Pool = 0, Map };

    template<typename AssetGroup>
    class AssetGroupIterator
    {
    public:
        using TypeVal = typename AssetGroup::AssetType;
        using TypeRef = typename Ref<TypeVal>;
        using IteratorType = typename AssetGroup::AssetGroupItem;

        using Pool = typename AssetGroup::GroupPool;
        using PoolIterator = typename Pool::iterator;

        using Map = typename AssetGroup::GroupMap;
        using MapIterator = typename Map::iterator;

        using IteratorVariant = std::variant<PoolIterator, MapIterator>;

    public:
        AssetGroupIterator() = default;
        AssetGroupIterator(PoolIterator it) : mIt(it) {}
        AssetGroupIterator(MapIterator it) : mIt(it) {}

        AssetGroupIterator& operator++()
        {
            std::visit([](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                arg++;
            }, mIt);

            return *this;
        }
        AssetGroupIterator operator++(int)
        {
            IteratorType temp = *this;
            std::visit([&temp](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp++;
            }, mIt);

            return temp;
        }
        AssetGroupIterator& operator--()
        {
            std::visit([](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                arg--;
            }, mIt);

            return *this;
        }
        AssetGroupIterator operator--(int)
        {
            IteratorType temp = *this;
            std::visit([&temp](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp--;
            }, mIt);

            return temp;
        }

        IteratorType& operator*()
        {
            IteratorType* temp = nullptr;

            std::visit([&temp](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp = &(*arg);
            }, mIt);

            LAB_CORE_ASSERT(temp, "Iterator pointer is invalid!");

            return *temp;
        }
        IteratorType* operator->()
        {
            IteratorType* temp = nullptr;

            std::visit([&temp](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp = &(*arg);
            }, mIt);

            return temp;
        }

        bool operator== (const AssetGroupIterator& other) const
        {
            LAB_CORE_ASSERT(mIt.index() == other.mIt.index(), "Iterator variants do not match!");

            bool match = false;

            std::visit([&match, other](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                match = arg == std::get<T>(other.mIt);
            }, mIt);

            return match;
        }
        bool operator!= (const AssetGroupIterator& other) const
        {
            return !(*this == other);
        }

    private:
        IteratorVariant mIt;
    };

    template<typename AssetGroup>
    class AssetGroupConstIterator
    {
    public:
        using Type = AssetGroupIterator<AssetGroup>;

        using TypeVal = typename AssetGroup::AssetType;
        using TypeRef = typename Ref<TypeVal>;
        using IteratorType = typename AssetGroup::AssetGroupItem;

        using Pool = typename AssetGroup::GroupPool;
        using PoolIterator = typename Pool::const_iterator;

        using Map = typename AssetGroup::GroupMap;
        using MapIterator = typename Map::const_iterator;

        using IteratorVariant = std::variant<PoolIterator, MapIterator>;

    public:
        AssetGroupConstIterator() = default;
        AssetGroupConstIterator(PoolIterator it) : mIt(it) {}
        AssetGroupConstIterator(MapIterator it) : mIt(it) {}

        AssetGroupConstIterator& operator++()
        {
            std::visit([](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                arg++;
            }, mIt);

            return *this;
        }
        AssetGroupConstIterator operator++(int)
        {
            IteratorType temp = *this;
            std::visit([&temp](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp++;
            }, mIt);

            return temp;
        }
        AssetGroupConstIterator& operator--()
        {
            std::visit([](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                arg--;
            }, mIt);

            return *this;
        }
        AssetGroupConstIterator operator--(int)
        {
            IteratorType temp = *this;
            std::visit([&temp](auto& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                        LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                    temp--;
                }, mIt);

            return temp;
        }

        const IteratorType& operator*() const
        {
            const IteratorType* temp = nullptr;

            std::visit([&temp](const auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp = &(*arg);
            }, mIt);

            LAB_CORE_ASSERT(temp, "Iterator pointer is invalid!");

            return *temp;
        }
        const IteratorType* operator->() const
        {
            const IteratorType* temp = nullptr;
             
            std::visit([&temp](const auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                temp = &(*arg);
            }, mIt);

            return temp;
        }

        bool operator== (const AssetGroupConstIterator& other) const
        {
            LAB_CORE_ASSERT(mIt.index() == other.mIt.index(), "Iterator variants do not match!");

            bool match = false;

            std::visit([&match, other](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, PoolIterator> || std::is_same_v<T, MapIterator>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                match = arg == std::get<T>(other.mIt);
            }, mIt);

            return match;
        }
        bool operator!= (const AssetGroupConstIterator& other) const
        {
            return !(*this == other);
        }

    private:
        IteratorVariant mIt;
    };

    template<typename T>
    class AssetGroup : public IAsset
    {
    public:
        using AssetType = T;
        using AssetRef = Ref<T>;
        using GroupRef = Ref<AssetGroup>;
        using AssetGroupItem = std::pair<const std::string, Ref<T>>;

        using GroupPool = std::vector<AssetGroupItem>;
        using GroupMap = std::unordered_map<std::string, AssetRef>;
        using GroupType = std::variant<GroupPool, GroupMap>;

        using Iterator = AssetGroupIterator<AssetGroup<T>>;
        using ConstIterator = AssetGroupConstIterator<AssetGroup<T>>;

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

        template<typename... Args>
        AssetRef addOrGet(const std::string& id, Args... args)
        {
            if (exists(id))
                return get(id);
            else
                return add(id, std::forward<Args>(args)...);
        }

        bool exists(const std::string& id)
        {
            bool result = false;
            std::visit([&id, &result](auto& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, GroupPool>)
                        result = std::find_if(arg.begin(), arg.end(), [&id](const AssetGroupItem& item) { return id == item.first; }) != arg.end();
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
                        asset = it->second;
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
                    auto it = std::find_if(arg.begin(), arg.end(), [&id](const AssetGroupItem& item) { return id == item.first; });
                    if (it != arg.end())
                        asset = it->second;
                }
                else if constexpr (std::is_same_v<T, GroupMap>)
                    asset = arg[id];
                else
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
            }, mAssets);

            return asset;
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

        constexpr size_t size() const
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

        size_t capacity() const
        {
            size_t capacity = 0;
            std::visit([&capacity](auto& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, GroupPool>)
                        capacity = arg.capacity();
                    else if constexpr (std::is_same_v<T, GroupMap>)
                        LAB_STATIC_ASSERT("Map storage does not have capacity function");
                    else
                        LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
                }, mAssets);

            return capacity;
        }

        void reserve(size_t newCapacity)
        {
            std::visit([&newCapacity](auto& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, GroupPool>)
                        arg.reserve(newCapacity);
                    else if constexpr (std::is_same_v<T, GroupMap>)
                        LAB_STATIC_ASSERT("Map storage does not have reserve function");
                    else
                        LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
                }, mAssets);
        }

        constexpr size_t ref_count() const
        {
            size_t ref_count = 0;
            std::visit([&ref_count](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, GroupPool> || std::is_same_v<T, GroupMap>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                for (const auto& [key, asset] : arg)
                    ref_count += AssetManager::GetRefCount(asset);

            }, mAssets);

            return ref_count;
        }

        Iterator begin()
        {
            Iterator it;
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, GroupPool> || std::is_same_v<T, GroupMap>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                it = arg.begin();
            }, mAssets);

            return it;
        }
        ConstIterator begin() const
        {
            ConstIterator it;
            std::visit([&it](const auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, GroupPool> || std::is_same_v<T, GroupMap>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                it = arg.begin();
            }, mAssets);

            return it;
        }
        Iterator end()
        {
            Iterator it;
            std::visit([&it](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, GroupPool> || std::is_same_v<T, GroupMap>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                it = arg.end();
            }, mAssets);

            return it;
        }
        ConstIterator end() const
        {
            ConstIterator it;
            std::visit([&it](const auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!(std::is_same_v<T, GroupPool> || std::is_same_v<T, GroupMap>))
                    LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");

                it = arg.end();
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

    template<typename... AssetGroupType>
    struct AssetGroups {};
    using AllAssetGroups = AssetGroups<
        Tex2DGroup, SubTex2DGroup, Tex2DSheetGroup
    >;

    using AssetGroupVariant = std::variant<
        Ref<Tex2DGroup>, Ref<SubTex2DGroup>, Ref<Tex2DSheetGroup>
    >;
}
