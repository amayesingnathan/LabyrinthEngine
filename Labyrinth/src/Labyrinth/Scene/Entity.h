#pragma once

#include "Scene.h"

#include "Labyrinth/Core/System/Cast.h"
#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Core/UUID.h"

namespace Labyrinth {

	//Defined here so it can be used in body of Entity.
	struct IDComponent
	{
		UUID id;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

		operator UUID() { return id; }
	};


	struct RootComponent;
	struct NodeComponent;

	class Entity
	{
	public:
		Entity() : mEntID(entt::null), mScene(nullptr) {}
		Entity(entt::entity entID, Ref<Scene> scene);
		//Entity(uint32_t entID, Scene* scene);

		Entity(const Entity& other) = default;
		~Entity() {}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(!hasComponent<T>(), "Can't add component that already exists on entity");
			T& component = mScene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
			mScene->onComponentAdded<T>(*this, component);
			return component;
		}

		//template<typename T>
		//T& addComponent(const T& component)
		//{
		//	assert(!hasComponent<T>());
		//	return mScene->mRegistry.emplace_or_replace<T>(mEntID, component);
		//}

		template<typename T>
		void removeComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't remove component that doesn't exist on entity");
			mScene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& getComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");
			return mScene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& getComponent() const
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");
			return mScene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		bool hasComponent() const
		{
			return mScene->mRegistry.all_of<T>(mEntID);
		}

		////Template to connect destruction of one component to the destruction of another.
		//template<typename TDestroy, typename TLink>
		//void connectDestruct()
		//{
		//	//m_Registry->on_destroy<TDestroy>().connect<&Entity::removeComponent<TLink>>();
		//	m_Registry->on_destroy<TDestroy>().connect<&Entity::removeComponent<TLink>>();
		//}

		uint32_t getEntID() const
		{
			return Cast<uint32_t>(mEntID);
		}

		UUID getUUID() const
		{
			return getComponent<IDComponent>().id;
		}

		operator entt::entity() const { return mEntID; }
		operator uint32_t() const { return Cast<uint32_t>(mEntID); }

		operator UUID() const { return getUUID(); }
		operator uint64_t() const { return getUUID(); }

		operator bool() const { return mEntID != entt::null; }

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

		void destroy() { mScene->DestroyEntity(*this); }
		Ref<Scene> getScene() { return mScene; }

		Entity& getParent();
		const Entity& getParent() const;
		bool hasParent();

		bool setParent(Entity& newParent, NodeComponent& node);
		bool setParent(Entity& newParent);

		std::vector<Entity>& getChildren();
		const std::vector<Entity>& getChildren() const;
    
		const size_t getChildCount() const { return getChildren().size(); }
		bool hasChild(const Entity& child) const;

		bool isRelated(const Entity& filter) const;

	private:
		void addChild(const Entity& child, NodeComponent& node);
		void addChild(const Entity& child);
		void removeChild(const Entity& child);

	private:
		entt::entity mEntID{ entt::null };
		Ref<Scene> mScene = nullptr;

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
		Entity parent = { entt::null, nullptr };
		std::vector<Entity> children = {};

		NodeComponent() = default;
		NodeComponent(const Entity& _parent, const std::vector<Entity>& _children = {})
			: parent(_parent), children(_children) {}

		operator bool() { return parent; }
	};
}