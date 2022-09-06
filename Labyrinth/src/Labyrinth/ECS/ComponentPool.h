#pragma once

#include "ECSTypes.h"

#include <Labyrinth/Core/System/Ref.h>

#include <vector>

namespace Labyrinth::ECS {

    template<typename T>
    struct Component
    {
        inline static const ComponentType Type = typeid(T);
    };
    
    class IComponentPool : public RefCounted
    {
    public:
        virtual ~IComponentPool() = default;
        virtual void tryDestroy(EntityID entity) = 0;
    };
    
    template<typename T>
    class ComponentPool : public IComponentPool
    {
    public:
        ComponentPool()
        {
            mEntityIndices.fill(-1);
        }
    
    private:
        void insert(EntityID entity, const T& component)
        {
            if (entity < 0)
                return;
            
            mEntityIndices[(usize)entity] = mEntityList.size();
            mEntityList.push_back(entity);
            mComponentList.emplace_back(component);
        }
        
        void tryDestroy(EntityID entity) override
        {
            if (entity < 0 || entity > MAX_ENTITIES)
                return; // Invalid entity
            
            // Get index of entity to destroy in Entity/Component lists. 
            i32 destroyIndex = mEntityIndices[(usize)entity];
            if (destroyIndex < 0)
                return; // Entity does not have this component
            
            // Place hole for this entity in mEntityIndices
            usize destroyUIndex = (usize)destroyIndex;
            mEntityIndices[(usize)entity] = -1;
            
            // Overwrite the data of entity to destroy with data from end of Entity/Component lists.
            EntityID lastEntity = mEntityList.back();
            mEntityList[destroyUIndex] = lastEntity;
            mComponentList[destroyUIndex] = std::move(mComponentList.back());

            // Set the index of the newly moved entity to the overwritten data;
            mEntityIndices[(usize)lastEntity] = destroyIndex;
            
            // Remove (now moved) data at end of lists.
            mEntityList.pop_back();
            mComponentList.pop_back();
        };
        
        T& get(EntityID entity)
        {
            LAB_CORE_ASSERT(entity > 0 && entity < MAX_ENTITIES, "Invalid entity!");
            
            i32 index = mEntityIndices[(usize)entity];
            LAB_CORE_ASSERT(index > 0, "Entity does not have component!");
            LAB_CORE_ASSERT(index < mComponentList.size(), "Internal error! Returned index is greater than number of components in pool");

            usize uindex = (usize)index;
            return mComponentList[uindex];
        }
        const T& get(EntityID entity) const
        {
            LAB_CORE_ASSERT(entity > 0 && entity < MAX_ENTITIES, "Invalid entity!");

            i32 index = mEntityIndices[(usize)entity];
            LAB_CORE_ASSERT(index > 0, "Entity does not have component!");
            LAB_CORE_ASSERT(index < mComponentList.size(), "Internal error! Returned index is greater than number of components in pool");

            usize uindex = (usize)index;
            return mComponentList[uindex];
        }
    
    private:
        std::array<i32, MAX_ENTITIES> mEntityIndices;
        std::vector<EntityID> mEntityList;
        std::vector<T> mComponentList;
        
        friend class ComponentManager;
    };
}