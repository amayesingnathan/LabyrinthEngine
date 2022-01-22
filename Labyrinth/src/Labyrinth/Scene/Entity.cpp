#include "Lpch.h"
#include "Entity.h"

namespace Labyrinth {

	using Children = std::unordered_set<Entity, Entity::HashFunction>;

	Entity::Entity(entt::entity entID, Scene* scene)
		: mEntID(entID), mScene(scene)
	{
	}

	Entity& Entity::getParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::hasParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::setParent(Entity newParent)
	{
		auto& node = getComponent<NodeComponent>();
		if (node.parent == newParent)
			return false;

		if (newParent)
			if (isRelated(newParent))
			{
				LAB_CORE_WARN("Cannot create circular ownership of entities!"); return false;
			}

		if (node.parent)
			node.parent.removeChild(*this);
		
		if (newParent)
			newParent.addChild(*this);

		node.parent = newParent;
		return true;
	}

	Children& Entity::getChildren() { return getComponent<NodeComponent>().children; }
	const Children& Entity::getChildren() const { return getComponent<NodeComponent>().children; }

	bool Entity::hasChild(const Entity& child) const 
	{ 
		return getChildren().find(child) != getChildren().end();
	}

	void Entity::addChild(Entity& child)
	{
		auto& node = getComponent<NodeComponent>();
		node.children.emplace(child);
	}

	void Entity::removeChild(Entity& child)
	{
		getChildren().erase(child);
	}

	bool Entity::isRelated(const Entity& filter) const
	{
		const auto& children = getComponent<NodeComponent>().children;
		// Cycles every child
		for (const auto& child : children)
		{
			if (child == filter)
			{
				// Found the child
				return true;
			}
			bool found = isRelated(filter);
			if (found)
				return found;
		}
		return false;
	}

}