#include "Lpch.h"
#include "Entity.h"

namespace Labyrinth {

	Entity::Entity(entt::entity entID, Ref<Scene> scene)
		: mEntID(entID), mScene(scene)
	{
	}

	//Entity::Entity(uint32_t entID, Scene* scene)
	//	: mEntID(Cast<entt::entity>(entID)), mScene(scene)
	//{
	//}

	//const Entity& Entity::getParent() const { return getComponent<NodeComponent>().parent; }
	Entity& Entity::getParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::hasParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::setParent(Entity& newParent, NodeComponent& node)
	{
		if (node.parent == newParent) return false;

		if (newParent)
		{
			if (!isRelated(newParent)) // Can only add to new parent's list of children if we're not already related.
				newParent.addChild(*this);
			else { LAB_CORE_WARN("Cannot create circular ownership of entities!"); return false; }
		}
		else { addComponent<RootComponent>(); };

		if (node.parent)
		{
			node.parent.removeChild(*this);
		}
		else
		{
			removeComponent<RootComponent>(); //No longer root entity (has parent)
		}

		node.parent = newParent;
		return true;
	}

	bool Entity::setParent(Entity& newParent)
	{
		return setParent(newParent, getComponent<NodeComponent>());
	}

	std::vector<Entity>& Entity::getChildren() { return getComponent<NodeComponent>().children; }
	const std::vector<Entity>& Entity::getChildren() const { return getComponent<NodeComponent>().children; }

	bool Entity::hasChild(const Entity& child) const 
	{ 
		const std::vector<Entity>& children = getChildren();
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
		std::vector<Entity>& children = getChildren();
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
			if (child == filter)
			{
				// Found the child
				return true;
			}
			bool found = child.isRelated(filter);
			if (found)
				return found;
		}
		return false;
	}
}