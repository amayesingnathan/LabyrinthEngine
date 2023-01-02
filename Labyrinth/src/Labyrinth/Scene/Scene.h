#pragma once

#include "Labyrinth/Assets/Asset.h"
#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Renderer/EditorCamera.h"
#include "Labyrinth/Renderer/RenderStack.h"

#include "ECS.h"

class b2World;

namespace Laby {

	class Entity;

	class Scene : public Asset
	{
	public:
		Scene(const std::string& name = "Untitled");
		~Scene();

		ASSET_STATIC_TYPE(AssetType::Scene)

		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, Entity parent);

		Entity CreateEntityWithID(UUID id, const std::string& name);
		Entity CreateEntityWithID(UUID id, const std::string& name, Entity parent);

		Entity CloneEntity(Entity copy);
		Entity CloneChild(Entity copy, Entity newParent);

		void DestroyEntity(Entity entity, bool linkChildren = false);

		Entity findEntity(UUID id);
		Entity getEntityByTag(const std::string& tag);
		Entity getChildByTag(const std::string& tag, Entity parent);

		template<typename... Components>
		auto getEntitiesWith()
		{
			return mRegistry.view<Components...>();
		}
		template<typename... Components>
		const auto getEntitiesWith() const
		{
			return mRegistry.view<Components...>();
		}

		void onRuntimeStart();
		void onRuntimeStop();

		void onSimulationStart();
		void onSimulationStop();

		void onUpdateRuntime(Timestep ts);
		void onUpdateSimulation(Timestep ts, EditorCamera& camera);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(u32 width, u32 height);

		Entity getPrimaryCameraEntity();

		void setName(const std::string name) { mName = name; }
		const std::string& getName() const { return mName; }
		bool hasName() const { return (mName != "Untitled" && mName != ""); }

		void transformChildren();

	private:
		void DestroyEntityR(Entity entity, Entity parent, bool linkChildren = false);

		void OnPhysicsStart();
		void OnPhysicsStop();

		void BuildScene();
		void DrawScene(EditorCamera& camera);
		void DrawScene(Camera& camera, const glm::mat4& transform);

		void StepPhysics2D(Timestep ts);

	private:
		std::string mName;

		Registry mRegistry;

		Single<RenderStack> mRenderStack = nullptr;

		b2World* mPhysicsWorld = nullptr;

		std::unordered_map<UUID, EntityID> mEntityMap;

		u32 mViewportWidth = 0, mViewportHeight = 0;

		friend Entity;
	};
}


