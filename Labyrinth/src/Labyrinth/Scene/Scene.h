#pragma once

#include "Labyrinth/Assets/Asset.h"
#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Renderer/EditorCamera.h"
#include "Labyrinth/Renderer/RenderStack.h"

#include "ECS.h"

namespace Laby {

	class Entity;
	struct TilemapComponent;

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
		Entity getEntityByTag(std::string_view tag);
		Entity getChildByTag(std::string_view tag, Entity parent);

		template<typename... Components>
		auto getEntitiesWith() { return mRegistry.view<Components...>(); }
		template<typename... Components>
		const auto getEntitiesWith() const { return mRegistry.view<Components...>(); }

		void onRuntimeStart();
		void onRuntimeStop();

		void onSimulationStart();
		void onSimulationStop();

		void onUpdateRuntime(Timestep ts);
		void onUpdateSimulation(Timestep ts, EditorCamera& camera);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(u32 width, u32 height);

		Entity getPrimaryCameraEntity();

		void setName(const std::string& name) { mName = name; }
		std::string& getName() { return mName; }
		const std::string& getName() const { return mName; }
		bool hasName() const { return (mName != "Untitled" && mName != ""); }

		void transformChildren();

	private:
		void DestroyEntityR(Entity entity, Entity parent, bool linkChildren = false);

		void CreateTilemapEntities();
		void CreateTilemapShapes(Entity mapEntity, const TilemapComponent& comp);
		void CreateTilemapScripts(Entity mapEntity, const TilemapComponent& comp);
		void CleanupTilemapEntities();

		void BuildScene();
		void DrawScene(EditorCamera& camera);
		void DrawScene(Camera& camera, const glm::mat4& transform);

		void StepPhysics2D(Timestep ts);
		void UpdateScripts(Timestep ts);

	private:
		void OnRigidBodyComponentConstruct(entt::registry& registry, entt::entity e);
		void OnRigidBodyComponentDestroy(entt::registry& registry, entt::entity e);
		void OnBoxColliderComponentConstruct(entt::registry& registry, entt::entity e);
		void OnBoxColliderComponentDestroy(entt::registry& registry, entt::entity e);
		void OnCircleColliderComponentConstruct(entt::registry& registry, entt::entity e);
		void OnCircleColliderComponentDestroy(entt::registry& registry, entt::entity e);
		void OnChainColliderComponentConstruct(entt::registry& registry, entt::entity e);
		void OnChainColliderComponentDestroy(entt::registry& registry, entt::entity e);

	private:
		std::string mName;
		u32 mViewportWidth = 0, mViewportHeight = 0;

		Registry mRegistry;
		Single<RenderStack> mRenderStack = nullptr;

		EntityID mSceneEntity;
		std::unordered_map<UUID, EntityID> mEntityMap;

		friend Entity;
		friend class SceneUtils;
	};

	class SceneUtils
	{
	public:
		static Ref<Scene> Clone(Ref<Scene> toCopy);
	};
}


