#pragma once

#include "ComponentPool.h"

#include <unordered_map>

namespace Labyrinth::ECS {

    class ComponentManager
    {
    public:
        ComponentManager() = default;

    private:
        template<typename T>
        bool HasComponentPool() const { return mPoolIndices.count(Component<T>::Type) != 0; }

        template<typename T>
        void RegisterType() const
        {
            LAB_CORE_ASSERT(!HasComponentPool<T>(), "Component type is already registered!");

            mPoolIndices.emplace(Component<T>::Type, mComponentPools.size());
            mComponentPools.push_back(Ref<ComponentPool<T>>::Create());
        }

        template<typename T, typename... Args>
        T& AddComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->insert(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->insert_or_replace(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T& ReplaceComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->replace(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent(EntityID entity)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->get(entity);
        }
        template<typename T>
        const T& GetComponent(EntityID entity) const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->get(entity);
        }

        template<typename T>
        void RemoveComponent(EntityID entity)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            pool->tryDestroy(entity);
        }

        template<typename T>
        bool HasComponent(EntityID entity) const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->exists(entity);
        }

        template<typename T>
        Ref<ComponentPool<T>> GetComponentPool() const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            ComponentIndex poolIndex = mPoolIndices.at(Component<T>::Type);
            return mComponentPools[poolIndex];
        }

        template<typename... T>
        std::tuple<Ref<ComponentPool<T>>...> GetComponentPools() const
        {
            return std::make_tuple<Ref<ComponentPool<T>>...>(ToTupleElement<T>()...);
        }

        void EntityDestroyed(EntityID entity)
        {
            for (Ref<IComponentPool> pool : mComponentPools)
                pool->tryDestroy(entity);
        };

        template<typename T>
        Ref<ComponentPool<T>> ToTupleElement() const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            ComponentIndex poolIndex = mPoolIndices.at(Component<T>::Type);
            return mComponentPools[poolIndex];
        }

    private:
        using ComponentIndex = usize;

        mutable std::vector<Ref<IComponentPool>> mComponentPools;
        mutable std::unordered_map<ComponentType, ComponentIndex> mPoolIndices;

        friend class Registry;
    };
}