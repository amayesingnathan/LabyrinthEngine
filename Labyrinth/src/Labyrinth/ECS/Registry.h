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

        template<typename T, typename... Args>
        T& emplace(EntityID entity, Args&&... args) { return mComponentManager->AddComponent<T>(entity, std::forward<Args>(args)...); }
        template<typename T, typename... Args>
        T& emplace_or_replace(EntityID entity, Args&&... args) { return mComponentManager->AddOrReplaceComponent<T>(entity, std::forward<Args>(args)...); }
        template<typename T, typename... Args>
        T& replace(EntityID entity, Args&&... args) { return mComponentManager->ReplaceComponent<T>(entity, std::forward<Args>(args)...); }

        template<typename T>
        void erase(EntityID entity) { mComponentManager->RemoveComponent<T>(entity); }

        template<typename... Component>
        ComponentView<Component...> view() const
        {
            auto pools = mComponentManager->GetComponentPools<Component...>();
            return ComponentView<Component...>(pools);
        }

        template<typename T>
        bool all_of(EntityID entity) const { return mComponentManager->HasComponent<T>(entity); }

        template<typename T>
        T& get(EntityID entity) { return mComponentManager->GetComponent<T>(entity); }
        template<typename T>
        const T& get(EntityID entity) const { return mComponentManager->GetComponent<T>(entity); }

    private:
        Single<EntityManager> mEntityManager;
        Single<ComponentManager> mComponentManager;
    };

}