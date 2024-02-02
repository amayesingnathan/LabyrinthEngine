#pragma once

#include "Scene.h"

#include "Labyrinth/Core/UUID.h"

namespace Laby {

	struct RootComponent;
	struct NodeComponent;
	struct TransformComponent;

	class Entity
	{
	public:
		Entity() : mEntID(NullEntity) {}
		Entity(EntityID entID, const Ref<Scene>& scene);

		Entity(const Entity& other) = default;
		~Entity() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(!hasComponent<T>(), "Can't add component that already exists on entity");

			Ref<Scene> scene = mScene.Lock();
			T& component = scene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			Ref<Scene> scene = mScene.Lock();
			T& component = scene->mRegistry.emplace_or_replace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& ReplaceComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't replace component that doesn't exist on entity");

			Ref<Scene> scene = mScene.Lock();
			T& component = scene->mRegistry.replace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		void RemoveComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't remove component that doesn't exist on entity");

			Ref<Scene> scene = mScene.Lock();
			scene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& GetComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.Lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& GetComponent() const
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.Lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		bool HasComponent() const
		{
			Ref<Scene> scene = mScene.Lock();
			return scene->mRegistry.all_of<T>(mEntID);
		}

		EntityID GetEntID() const { return mEntID; }

		UUID GetUUID() const;

		TransformComponent& GetTransform();
		const TransformComponent& GetTransform() const;

		std::string& GetTag();
		const std::string& GetTag() const;

		operator bool() const { return Valid(); }
		bool Valid() const { return mEntID != NullEntity && mScene.Valid() && mScene->mRegistry.valid(mEntID); }

		auto operator<=>(const Entity& other) const { return GetUUID() <=> other.GetUUID(); }
		bool operator==(const Entity& other) const { return GetUUID() == other.GetUUID(); }
		
		void Destroy();
		Ref<Scene> GetScene() { return mScene.Lock(); }

		Entity GetParent() const;
		bool HasParent();

		bool SetParent(Entity newParent, NodeComponent& node);
		bool SetParent(Entity newParent);

		std::vector<UUID>& GetChildren();
		const std::vector<UUID>& GetChildren() const;
		void RemoveChildren();

		Entity FindChild(std::string_view tag);

		const usize GetChildCount() const { return GetChildren().size(); }
		bool HasChild(Entity child) const;

		bool IsRelated(Entity filter) const;

	private:
		void AddChild(Entity child, NodeComponent& node);
		void AddChild(Entity child);
		void RemoveChild(Entity child);

	private:
		EntityID mEntID{ NullEntity };
		WeakRef<Scene> mScene;

		friend Scene;
		friend NodeComponent;
	};
}