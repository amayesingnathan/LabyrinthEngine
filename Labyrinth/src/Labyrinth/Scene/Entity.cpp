#include "Lpch.h"
#include "Entity.h"

#include "Components.h"

namespace Labyrinth {

	Entity::Entity(entt::entity entID, const Ref<Scene>& scene)
		: mEntID(entID), mScene(scene)
	{
	}

	Entity Entity::getParent() const { return mScene.lock()->FindEntity(getComponent<NodeComponent>().parent); }
	const UUID& Entity::getUUID() const { return getComponent<IDComponent>().id; }

	void Entity::destroy()
	{
		Ref<Scene> scene = mScene.lock();
		scene->DestroyEntity(*this);
		mEntID = entt::null; 
		scene = nullptr;
	}


	bool Entity::hasParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::setParent(Entity newParent, NodeComponent& node)
	{
		Entity currentParent = mScene.lock()->FindEntity(newParent ? newParent : 0);
		if (currentParent == newParent) return false;

		if (newParent)
		{
			if (!isRelated(newParent)) // Can only add to new parent's list of children if we're not already related.
				newParent.addChild(*this);
			else { LAB_CORE_WARN("Cannot create circular ownership of entities!"); return false; }
		}
		else { addComponent<RootComponent>(); };

		if (currentParent)
			currentParent.removeChild(*this);
		else
			removeComponent<RootComponent>(); //No longer root entity (will have parent)

		node.parent = newParent;
		return true;
	}

	bool Entity::setParent(Entity newParent)
	{
		return setParent(newParent, getComponent<NodeComponent>());
	}

	std::vector<UUID>& Entity::getChildren() { return getComponent<NodeComponent>().children; }
	const std::vector<UUID>& Entity::getChildren() const { return getComponent<NodeComponent>().children; }

	bool Entity::hasChild(Entity child) const 
	{ 
		const std::vector<UUID>& children = getChildren();
		auto it = std::find(children.begin(), children.end(), child.getUUID());
		return it != getChildren().end();
	}

	void Entity::addChild(Entity child, NodeComponent& node)
	{
		node.children.emplace_back(child);
	}

	void Entity::addChild(Entity child)
	{
		addChild(child, getComponent<NodeComponent>());
	}

	void Entity::removeChild(Entity child)
	{
		std::vector<UUID>& children = getChildren();
		auto it = std::find(children.begin(), children.end(), child.getUUID());
		if (it != children.end())
			children.erase(it);
	}

	bool Entity::isRelated(Entity filter) const
	{
		const auto& children = getComponent<NodeComponent>().children;
		// Cycles every child
		for (const auto& child : children)
		{
			Entity childEnt = mScene.lock()->FindEntity(child);
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