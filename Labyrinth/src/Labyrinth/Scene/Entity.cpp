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

	bool Entity::hasChild(Entity& child) { return getChildren().find(child) != getChildren().end(); }

	void Entity::addChild(Entity& child)
	{
		auto& node = getComponent<NodeComponent>();
		node.children.emplace(child);
	}

	void Entity::removeChild(Entity& child)
	{
		getChildren().erase(child);
	}

	bool Entity::isRelated(Entity& filter)
	{
		auto& children = getComponent<NodeComponent>().children;
		// cycles every child
		for (auto child : children)
		{
			if (child == filter)
			{
				// returns the desired child
				return true;
			}
			bool found = isRelated(filter);
			if (found)
				return found;
		}
		return false;
	}




	NodeComponent::NodeComponent(const Entity& _parent, const Children& _children)
		: parent(_parent), children(_children) {}

}