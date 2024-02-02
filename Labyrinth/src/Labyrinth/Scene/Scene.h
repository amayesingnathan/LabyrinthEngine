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

	namespace ECS { class Groups; }

	class Scene : public Asset
	{
	public:
		Scene(const std::string& name = "Untitled");
		~Scene();

		ASSET_METADATA(AssetType::Scene, "scenes")

		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, Entity parent);

		Entity CreateEntityWithID(UUID id, const std::string& name);
		Entity CreateEntityWithID(UUID id, const std::string& name, Entity parent);

		Entity CloneEntity(Entity copy);
		Entity CloneChild(Entity copy, Entity newParent);

		void DestroyEntity(Entity entity, bool linkChildren = false);

		Entity FindEntity(UUID id);
		Entity GetEntityByTag(std::string_view tag);
		Entity GetChildByTag(std::string_view tag, Entity parent);

		template<typename... Components>
		auto GetEntitiesWith() { return mRegistry.view<Components...>(); }
		template<typename... Components>
		const auto GetEntitiesWith() const { return mRegistry.view<Components...>(); }

		template<typename T, typename... R>
		auto group() { return mRegistry.group<T>(entt::get<R...>); }
		template<typename T, typename... R>
		const auto group() const { return mRegistry.group<T>(entt::get<R...>); }

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		void OnViewportResize(u32 width, u32 height);

		Entity GetPrimaryCameraEntity();

		void SetName(const std::string& name) { mName = name; }
		std::string& GetName() { return mName; }
		const std::string& GetName() const { return mName; }
		bool HasName() const { return (mName != "Untitled" && mName != ""); }

		void TransformChildren();

	private:
		void DestroyEntityR(Entity entity, Entity parent, bool linkChildren = false);

		void CreateTilemapEntities();
		void CreateTilemapShapes(Entity mapEntity, const TilemapComponent& comp);
		void CreateTilemapScripts(Entity mapEntity, const TilemapComponent& comp);
		void CleanupTilemapEntities();

		void ResetAnimations(bool start = false);

		void BuildScene();
		void DrawScene(EditorCamera& camera);
		void DrawScene(Camera& camera, const glm::mat4& transform);

		void StepPhysics2D(Timestep ts);
		void UpdateScripts(Timestep ts);
		void UpdateAnimation();

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
		Single<ECS::Groups> mGroups;

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


