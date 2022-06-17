#include "Lpch.h"
#include "Entity.h"

namespace Labyrinth {

	Entity::Entity(entt::entity entID, Ref<Scene> scene)
		: mEntID(entID), mScene(scene)
	{
	}

	const entt::entity& Entity::getParent() const { return getComponent<NodeComponent>().parent; }
	entt::entity& Entity::getParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::hasParent() { return getComponent<NodeComponent>(); }

	bool Entity::setParent(Entity newParent, NodeComponent& node)
	{
		if (node.parent == newParent) return false;

		if (newParent)
		{
			if (!isRelated(newParent)) // Can only add to new parent's list of children if we're not already related.
				newParent.addChild(*this);
			else { LAB_CORE_WARN("Cannot create circular ownership of entities!"); return false; }
		}
		else { addComponent<RootComponent>(); };

		if (node)
		{
			Entity nodeEnt(node.parent, mScene);
			nodeEnt.removeChild(*this);
		}
		else removeComponent<RootComponent>(); //No longer root entity (will have parent)

		node.parent = newParent;
		return true;
	}

	bool Entity::setParent(Entity newParent)
	{
		return setParent(newParent, getComponent<NodeComponent>());
	}

	std::vector<entt::entity>& Entity::getChildren() { return getComponent<NodeComponent>().children; }
	const std::vector<entt::entity>& Entity::getChildren() const { return getComponent<NodeComponent>().children; }

	bool Entity::hasChild(const Entity& child) const 
	{ 
		const std::vector<entt::entity>& children = getChildren();
		auto it = std::find(children.begin(), children.end(), child);
		return it != getChildren().end();
	}

	void Entity::addChild(const Entity& child, NodeComponent& node)
	{
		node.children.emplace_back(child);
	}

	void Entity::addChild(const Entity& child)
	{
		addChild(child, getComponent<NodeComponent>());
	}

	void Entity::removeChild(const Entity& child)
	{
		std::vector<entt::entity>& children = getChildren();
		auto it = std::find(children.begin(), children.end(), child);
		if (it != children.end())
			children.erase(it);
	}

	bool Entity::isRelated(const Entity& filter) const
	{
		const auto& children = getComponent<NodeComponent>().children;
		// Cycles every child
		for (const auto& child : children)
		{
			Entity childEnt(child, mScene);
			if (childEnt == filter)
			{
				// Found the child
				return true;
			}
			bool found = childEnt.isRelated(filter);
			if (found)
				return found;
		}
		return false;
	}
}