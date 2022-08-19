#pragma once

#include "Scene.h"

#include "Labyrinth/Core/System/Cast.h"
#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Core/UUID.h"

namespace Labyrinth {

	struct RootComponent;
	struct NodeComponent;
	struct TransformComponent;

	class Entity
	{
	public:
		Entity() : mEntID(entt::null) {}
		Entity(entt::entity entID, const Ref<Scene>& scene);

		Entity(const Entity& other) = default;
		~Entity() {}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(!hasComponent<T>(), "Can't add component that already exists on entity");

			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
			scene->onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args)	
		{
			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.emplace_or_replace<T>(mEntID, std::forward<Args>(args)...);
			scene->onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& replaceComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't replace component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.replace<T>(mEntID, std::forward<Args>(args)...);
			scene->onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		void removeComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't remove component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			scene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& getComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& getComponent() const
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		bool hasComponent() const
		{
			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.all_of<T>(mEntID);
		}

		u32 getEntID() const
		{
			return Cast<u32>(mEntID);
		}

		const UUID& getUUID() const;
		
		TransformComponent& getTransform();
		const TransformComponent& getTransform() const;

		operator entt::entity() const { return mEntID; }
		operator u32() const { return Cast<u32>(mEntID); }

		operator UUID() const { return getUUID(); }
		operator u64() const { return getUUID(); }

		operator bool() const { return (mEntID != entt::null && mScene.valid()); }

		bool operator==(const Entity& other) const
		{
			// Be sure to check for cases involving null entity because we can't do getUUID() on null entity.
			if (!*this != !other) return false;
			if (!*this && !other) return true;

			return getUUID() == other.getUUID();
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		void destroy();
		Ref<Scene> getScene() { return mScene.lock(); }

		Entity getParent() const;
		bool hasParent();

		bool setParent(Entity newParent, NodeComponent& node);
		bool setParent(Entity newParent);

		std::vector<UUID>& getChildren();
		const std::vector<UUID>& getChildren() const;
		void removeChildren();
    
		const usize getChildCount() const { return getChildren().size(); }
		bool hasChild(Entity child) const;

		bool isRelated(Entity filter) const;

	private:
		void addChild(Entity child, NodeComponent& node);
		void addChild(Entity child);
		void removeChild(Entity child);

	private:
		entt::entity mEntID{ entt::null };
		WeakRef<Scene> mScene;

		friend Scene;
		friend NodeComponent;
	};
}