#pragma once
#include "Labyrinth.h"
#include "entt.hpp"

class Entity
{
public:
	Entity(entt::entity entID, Labyrinth* game) : m_EntID(entID), m_Game(game) {};
	Entity(const Entity& other) = default;

	template<typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		assert(!hasComponent<T>());
		return m_Game->m_Registry.emplace<T>(m_EntID, std::forward<Args>(args));
	}

	template<typename T>
	void removeComponent()
	{
		assert(hasComponent<T>());
		m_Game->m_Registry.erase<T>(m_EntID);
	}

	template<typename T>
	T& getComponent()
	{
		assert(hasComponent<T>());
		return m_Game->m_Registry.get<T>(m_EntID);
	}

	template<typename T>
	bool hasComponent()
	{
		return m_Game->m_Registry.all_of<T>(m_EntID);
	}

	entt::entity getID() { return m_EntID; };

	Entity& operator=(const Entity& other)
	{
		// Guard self assignment
		if (this == &other)
			return *this;

		m_EntID = other.m_EntID;
		m_Game = other.m_Game;
	}

private:
	entt::entity m_EntID;
	Labyrinth* m_Game;

};