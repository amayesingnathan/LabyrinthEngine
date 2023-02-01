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
		T& addComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(!hasComponent<T>(), "Can't add component that already exists on entity");

			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.emplace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args)
		{
			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.emplace_or_replace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& replaceComponent(Args&&... args)
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't replace component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			T& component = scene->mRegistry.replace<T>(mEntID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		void removeComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't remove component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			scene->mRegistry.erase<T>(mEntID);
		}

		template<typename T>
		T& getComponent()
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		const T& getComponent() const
		{
			LAB_CORE_ASSERT(hasComponent<T>(), "Can't get component that doesn't exist on entity");

			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.get<T>(mEntID);
		}

		template<typename T>
		bool hasComponent() const
		{
			Ref<Scene> scene = mScene.lock();
			return scene->mRegistry.all_of<T>(mEntID);
		}

		EntityID getEntID() const { return mEntID; }

		UUID getUUID() const;

		TransformComponent& getTransform();
		const TransformComponent& getTransform() const;

		std::string& getTag();
		const std::string& getTag() const;

		operator bool() const { return valid(); }
		bool valid() const { return mEntID != NullEntity && mScene.valid(); }

		auto operator<=>(const Entity& other) const { return getUUID() <=> other.getUUID(); }
		bool operator==(const Entity& other) const { return getUUID() == other.getUUID(); }
		
		void destroy();
		Ref<Scene> getScene() { return mScene.lock(); }

		Entity getParent() const;
		bool hasParent();

		bool setParent(Entity newParent, NodeComponent& node);
		bool setParent(Entity newParent);

		std::vector<UUID>& getChildren();
		const std::vector<UUID>& getChildren() const;
		void removeChildren();

		const usize getChildCount() const { return getChildren().size(); }
		bool hasChild(Entity child) const;

		bool isRelated(Entity filter) const;

	private:
		void addChild(Entity child, NodeComponent& node);
		void addChild(Entity child);
		void removeChild(Entity child);

	private:
		EntityID mEntID{ NullEntity };
		WeakRef<Scene> mScene;

		friend Scene;
		friend NodeComponent;
	};
}