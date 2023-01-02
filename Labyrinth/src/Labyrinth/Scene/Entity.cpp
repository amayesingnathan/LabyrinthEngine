#include "Lpch.h"
#include "Entity.h"

#include "Components.h"

namespace Laby {

	Entity::Entity(EntityID entID, const Ref<Scene>& scene)
		: mEntID(entID), mScene(scene)
	{
	}

	Entity Entity::getParent() const { return mScene.lock()->findEntity(getComponent<NodeComponent>().parent); }
	const UUID& Entity::getUUID() const { return getComponent<IDComponent>().id; }

	TransformComponent& Entity::getTransform() { return getComponent<TransformComponent>(); }
	const TransformComponent& Entity::getTransform() const { return getComponent<TransformComponent>(); }

	std::string& Entity::getTag() { return getComponent<TagComponent>().tag; }
	const std::string& Entity::getTag() const { return getComponent<TagComponent>().tag; }

	void Entity::destroy()
	{
		mScene->DestroyEntity(*this);
		mEntID = NullEntity;
		mScene = nullptr;
	}

	bool Entity::hasParent() { return getComponent<NodeComponent>().parent; }

	bool Entity::setParent(Entity newParent, NodeComponent& node)
	{
		Entity currentParent = mScene->findEntity(node.parent);
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
		node.children.emplace_back(child.getUUID());
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

	void Entity::removeChildren()
	{
		std::vector<UUID>& children = getChildren();
		for (UUID id : children)
		{
			Entity child = mScene->findEntity(id);
			child.addComponent<RootComponent>();

			child.getComponent<NodeComponent>().parent = 0;
		}

		children.clear();
	}

	bool Entity::isRelated(Entity filter) const
	{
		const auto& children = getComponent<NodeComponent>().children;
		// Cycles every child
		for (const auto& child : children)
		{
			Entity childEnt = mScene.lock()->findEntity(child);
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