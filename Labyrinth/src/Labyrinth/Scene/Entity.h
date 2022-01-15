#pragma once

#include "Scene.h"

#include "Labyrinth/Core/Log.h"

namespace Labyrinth {

	struct NodeComponent;

	class Entity
	{
	public:
		struct HashFunction
		{
			size_t operator()(const Entity& entity) const
			{
				size_t xHash = std::hash<uint32_t>()((uint32_t)entity.mEntID);
				size_t yHash = std::hash<uint64_t>()(reinterpret_cast<uint64_t>(entity.mScene)) << 1;
				return xHash ^ yHash;
			}
		};

	public:
		Entity() : mEntID(entt::null), mScene(nullptr) {}
		Entity(entt::entity entID, Scene* scene);

		Entity(const Entity& other) = default;
		~Entity() {}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			assert(!hasComponent<T>());
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
			assert(hasComponent<T>());
			mScene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& getComponent()
		{
			assert(hasComponent<T>());
			return mScene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& getComponent() const
		{
			assert(hasComponent<T>());
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

		Scene* getScene() { return mScene; }

		Entity& getParent();
		bool hasParent();

		bool setParent(Entity newParent = {entt::null, nullptr});

		std::unordered_set<Entity, Entity::HashFunction>& getChildren();
		const std::unordered_set<Entity, Entity::HashFunction>& getChildren() const;
		bool hasChild(const Entity& child) const;

		bool isRelated(const Entity& filter) const;

	private:
		void addChild(Entity& child);
		void removeChild(Entity& child);

	private:
		entt::entity mEntID{ entt::null };
		Scene* mScene;

	};

	using Children = std::unordered_set<Entity, Entity::HashFunction>;

	//Node component for use in parent/child relations
	struct NodeComponent
	{
		Entity parent = { entt::null, nullptr };
		Children children = {};

		NodeComponent() = default;
		NodeComponent(const Entity& _parent, const Children& _children = {})
		: parent(_parent), children(_children) {}

		operator bool() { return parent; }
	};
}