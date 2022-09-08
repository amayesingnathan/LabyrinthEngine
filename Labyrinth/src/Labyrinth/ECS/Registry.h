#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "ComponentView.h"

namespace Labyrinth::ECS {

    class Registry
    {
    public:
        Registry()
        {
            mEntityManager = CreateSingle<EntityManager>();
            mComponentManager = CreateSingle<ComponentManager>();
        }

        EntityID create() { return mEntityManager->Create(); }
        void destroy(EntityID entity)
        {
            mEntityManager->Destroy(entity);
            mComponentManager->EntityDestroyed(entity);
        }

        template<typename T>
        void emplace(EntityID entity, const T& component = T()) { mComponentManager->AddComponent<T>(entity, component); }

        template<typename T>
        void remove(EntityID entity) { mComponentManager->RemoveComponent<T>(entity); }

        template<typename... Component>
        ComponentView<Component...> view() 
        {
            auto pools = mComponentManager->GetComponentPools<Component...>();
            return ComponentView(pools);
        }

        template<typename T>
        T& get(EntityID entity) { mComponentManager->GetComponent<T>(entity); }
        template<typename T>
        const T& get(EntityID entity) const { mComponentManager->GetComponent<T>(entity); }

    private:
        Single<EntityManager> mEntityManager;
        Single<ComponentManager> mComponentManager;
    };

}