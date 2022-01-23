#pragma once

#include "Scene.h"

#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Core/System/Log.h"

namespace Labyrinth {

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
			LAB_CORE_ASSERT(!hasComponent<T>(), "Can't add component that already exists on component");
			T& component = mScene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
			//mScene->onComponentAdded<T>(*this, component);
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
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't remove component that doesn't exist on component");
			mScene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& getComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on component");
			return mScene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& getComponent() const
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on component");
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

		uint32_t getID() const
		{
			return static_cast<uint32_t>(mEntID);
		}

		operator entt::entity() const { return mEntID; }
		operator uint32_t() const { return static_cast<uint32_t>(mEntID); }

		operator bool() const { return mEntID != entt::null; }

		bool operator==(const Entity& other) const
		{
			return mEntID == other.mEntID && mScene == other.mScene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		bool operator!=(const entt::entity& other) const
		{
			return !(getID() == static_cast<uint32_t>(other));
		}

		Ref<Scene> getScene() { return mScene; }

		Entity& getParent();
		bool hasParent();

		bool setParent(Entity& newParent, NodeComponent& node);
		bool setParent(Entity& newParent);

		std::vector<Entity>& getChildren();
		const std::vector<Entity>& getChildren() const;
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

	};

	//Root components just indicates an entity has no parent.
	struct RootComponent 
	{
		// Seem to need some actual data to be able to use as template parameter in addComponent
		// so just added a random zero byte.
		uint8_t data = 0; 
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