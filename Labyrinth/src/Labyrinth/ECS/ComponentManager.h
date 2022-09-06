#pragma once

#include "ComponentPool.h"

#include <Labyrinth/Core/System/Assert.h>

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
        void RegisterType()
        {
            LAB_CORE_ASSERT(!HasComponentPool<T>(), "Component type is already registered!");

            mPoolIndices.insert(Component<T>::Type, mComponentPools.size());
            mComponentPools.push_back(Ref<ComponentPool<T>>::Create());
        }

        template<typename T>
        void AddComponent(EntityID entity, const T& component)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            Ref<T> pool = GetComponentPool<T>();
            pool->insert(entity, component);
        }

        template<typename T>
        T& GetComponent(EntityID entity)
        {
            Ref<T> pool = GetComponentPool<T>();
            return pool->get(entity);
        }
        template<typename T>
        const T& GetComponent(EntityID entity) const
        {
            Ref<T> pool = GetComponentPool<T>();
            return pool->get(entity);
        }

        template<typename T>
        void RemoveComponent(EntityID entity)
        {
            Ref<T> pool = GetComponentPool<T>();
            pool->destroy(entity);
        }

        template<typename T>
        Ref<T> GetComponentPool()
        {
            LAB_CORE_ASSERT(HasComponentPool<T>(), "Manager does not have component type registered!");

            ComponentIndex poolIndex = mPoolIndices[Component<T>::Type];
            return mComponentPools[poolIndex];
        }

        void EntityDestroyed(EntityID entity)
        {
            for (Ref<IComponentPool> pool : mComponentPools)
                pool->tryDestroy(entity);
        };

    private:
        using ComponentIndex = usize;

        std::vector<Ref<IComponentPool>> mComponentPools;
        std::unordered_map<ComponentType, ComponentIndex> mPoolIndices;

        friend class Registry;
    };
}