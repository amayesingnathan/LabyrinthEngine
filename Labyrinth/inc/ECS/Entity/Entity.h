#pragma once

#include "Scene.h"

#include "ECS/Entity/entt.hpp"

class Entity
{
public:
	Entity() : mEntID(entt::null), mScene(nullptr) {}
	Entity(entt::entity entID, Scene* scene) : mEntID(entID), mScene(scene) {}
	Entity(const Entity& other) = default;


	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		assert(!hasComponent<T>());
		return mScene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
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
	bool hasComponent()
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

	entt::entity getID() { return mEntID; };

private:
	entt::entity mEntID;
	Scene* mScene;

};