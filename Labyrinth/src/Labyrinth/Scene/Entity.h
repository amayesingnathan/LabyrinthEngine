#pragma once

#include "Scene.h"

#include "Labyrinth/Core/System/Cast.h"
#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Core/UUID.h"

namespace Labyrinth {

	struct RootComponent;
	struct NodeComponent;

	class LAB_API Entity
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

		uint32_t getEntID() const
		{
			return Cast<uint32_t>(mEntID);
		}

		const UUID& getUUID() const;

		operator entt::entity() const { return mEntID; }
		operator uint32_t() const { return Cast<uint32_t>(mEntID); }

		operator UUID() const { return getUUID(); }
		operator uint64_t() const { return getUUID(); }

		operator bool() const { return (mEntID != entt::null && !mScene.expired()); }

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

		Entity& getParent();
		const Entity& getParent() const;
		bool hasParent();

		bool setParent(Entity newParent, NodeComponent& node);
		bool setParent(Entity newParent);

		std::vector<Entity>& getChildren();
		const std::vector<Entity>& getChildren() const;
    
		const size_t getChildCount() const { return getChildren().size(); }
		bool hasChild(const Entity& child) const;

		bool isRelated(const Entity& filter) const;

		void addChild(const Entity& child, NodeComponent& node);
		void addChild(const Entity& child);
		void removeChild(const Entity& child);

	private:
		entt::entity mEntID{ entt::null };
		WeakRef<Scene> mScene;

		friend Scene;
		friend NodeComponent;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//   Components exclusively for interaction with and between entities are stored here instead of Components.h	//
	//   as they need the full Entity definition, and we can't to include Entity.h in Components.h					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Root components just indicates an entity has no parent.
	struct RootComponent
	{
		// Seem to need some actual data to be able to use as template parameter in addComponent
		// so just added a random zero byte.
		uint8_t data = 0x0;
		RootComponent() = default;
	};

	//Node component for use in parent/child relations
	struct NodeComponent
	{
		Entity parent = {};
		std::vector<Entity> children = {};

		NodeComponent() = default;
		NodeComponent(const Entity& _parent, const std::vector<Entity>& _children = {})
			: parent(_parent), children(_children) {}

		operator bool() { return parent; }
	};
}