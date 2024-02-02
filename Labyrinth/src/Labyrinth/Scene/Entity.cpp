#include "Lpch.h"
#include "Entity.h"

#include "Components.h"

namespace Laby {

	Entity::Entity(EntityID entID, const Ref<Scene>& scene)
		: mEntID(entID), mScene(scene)
	{
	}

	Entity Entity::GetParent() const { return mScene.Lock()->FindEntity(GetComponent<NodeComponent>().parent); }
	UUID Entity::GetUUID() const 
	{ 
		if (!Valid())
			return 0;

		return GetComponent<IDComponent>().id; 
	}

	TransformComponent& Entity::GetTransform() { return GetComponent<TransformComponent>(); }
	const TransformComponent& Entity::GetTransform() const { return GetComponent<TransformComponent>(); }

	std::string& Entity::GetTag() { return GetComponent<TagComponent>().tag; }
	const std::string& Entity::GetTag() const { return GetComponent<TagComponent>().tag; }

	void Entity::Destroy()
	{
		mScene->DestroyEntity(*this);
		mEntID = NullEntity;
		mScene = nullptr;
	}

	bool Entity::HasParent() { return GetComponent<NodeComponent>().parent; }

	bool Entity::SetParent(Entity newParent, NodeComponent& node)
	{
		Entity currentParent = mScene->FindEntity(node.parent);
		if (currentParent == newParent) return false;

		if (newParent)
		{
			if (!IsRelated(newParent)) // Can only Add to new parent's list of children if we're not already related.
				newParent.AddChild(*this);
			else { LAB_CORE_WARN("Cannot create circular ownership of entities!"); return false; }
		}
		else { AddComponent<RootComponent>(); };

		if (currentParent)
			currentParent.RemoveChild(*this);
		else
			RemoveComponent<RootComponent>(); //No longer root entity (will have parent)

		node.parent = newParent.GetUUID();
		return true;
	}

	bool Entity::SetParent(Entity newParent)
	{
		return SetParent(newParent, GetComponent<NodeComponent>());
	}

	std::vector<UUID>& Entity::GetChildren() { return GetComponent<NodeComponent>().children; }
	const std::vector<UUID>& Entity::GetChildren() const { return GetComponent<NodeComponent>().children; }

	bool Entity::HasChild(Entity child) const
	{
		const std::vector<UUID>& children = GetChildren();
		auto it = std::find(children.begin(), children.end(), child.GetUUID());
		return it != GetChildren().end();
	}

	void Entity::AddChild(Entity child, NodeComponent& node)
	{
		node.children.emplace_back(child.GetUUID());
	}

	void Entity::AddChild(Entity child)
	{
		AddChild(child, GetComponent<NodeComponent>());
	}

	void Entity::RemoveChild(Entity child)
	{
		std::vector<UUID>& children = GetChildren();
		auto it = std::find(children.begin(), children.end(), child.GetUUID());
		if (it != children.end())
			children.erase(it);
	}

	void Entity::RemoveChildren()
	{
		std::vector<UUID>& children = GetChildren();
		for (UUID id : children)
		{
			Entity child = mScene->FindEntity(id);
			child.AddComponent<RootComponent>();

			child.GetComponent<NodeComponent>().parent = 0;
		}

		children.clear();
	}

	Entity Entity::FindChild(std::string_view tag)
	{
		if (!Valid())
			return Entity{};

		for (auto e : GetChildren())
		{
			Entity entity = mScene.Lock()->FindEntity(e);
			if (entity.GetComponent<TagComponent>().tag == tag)
				return entity;

			Entity checkChildren = entity.FindChild(tag);
			if (checkChildren)
				return checkChildren;
		}

		return Entity{};
	}

	bool Entity::IsRelated(Entity filter) const
	{
		const auto& children = GetComponent<NodeComponent>().children;
		// Cycles every child
		for (const auto& child : children)
		{
			Entity childEnt = mScene.Lock()->FindEntity(child);
			if (childEnt == filter)
			{
				// Found the child
				return true;
			}
			bool found = childEnt.IsRelated(filter);
			if (found)
				return found;
		}
		return false;
	}
}