#pragma once

#include "ECS/entt.hpp"

class Entity
{
public:
	Entity() : m_EntID(entt::null), m_Registry(nullptr) {}
	Entity(entt::entity entID, entt::registry* reg) : m_EntID(entID), m_Registry(reg) {}
	Entity(const Entity& other) = default;


	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		assert(!hasComponent<T>());
		return m_Registry->emplace<T>(m_EntID, std::forward<Args>(args)...);
	}

	template<typename T>
	void removeComponent()
	{
		assert(hasComponent<T>());
		m_Registry->erase<T>(m_EntID);
	}

	template<typename T>
	T& getComponent()
	{
		assert(hasComponent<T>());
		return m_Registry->get<T>(m_EntID);
	}

	template<typename T>
	bool hasComponent()
	{
		return m_Registry->all_of<T>(m_EntID);
	}

	////Template to connect destruction of one component to the destruction of another.
	//template<typename TDestroy, typename TLink>
	//void connectDestruct()
	//{
	//	//m_Registry->on_destroy<TDestroy>().connect<&Entity::removeComponent<TLink>>();
	//	m_Registry->on_destroy<TDestroy>().connect<&Entity::removeComponent<TLink>>();
	//}

	entt::entity getID() { return m_EntID; };

private:
	entt::entity m_EntID;
	entt::registry* m_Registry;

};