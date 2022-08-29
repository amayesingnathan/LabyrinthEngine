#pragma once

#include "Labyrinth/Assets/Asset.h"
#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Renderer/EditorCamera.h"
#include "Labyrinth/Renderer/RenderStack.h"

#include "entt.hpp"

class b2World;

namespace Labyrinth {

	class Entity;
	class ContactListener;

	class Scene : public Asset
	{
	public:
		Scene(const std::string& name = "Untitled");
		~Scene();

		ASSET_STATIC_TYPE(AssetType::Scene)

		Ref<Scene> Clone();

		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, Entity parent);

		Entity CreateEntityWithID(const UUID& id, const std::string& name);
		Entity CreateEntityWithID(const UUID& id, const std::string& name, Entity parent);

		Entity CloneEntity(Entity copy);
		Entity CloneChild(Entity copy, Entity newParent);

		void DestroyEntity(Entity entity, bool linkChildren = false);

		Entity findEntity(UUID id);
		Entity getEntityByTag(const std::string& tag);
		Entity getChildByTag(const std::string& tag, Entity parent);

		template<typename Component, typename... Other, typename... Exclude>
		auto getEntitiesWith(entt::exclude_t<Exclude...> = {})
		{
			return mRegistry.view<Component, Other...>(entt::exclude<Exclude...>);
		}
		template<typename Component, typename... Other, typename... Exclude>
		const auto getEntitiesWith(entt::exclude_t<Exclude...> = {}) const
		{
			return mRegistry.view<Component, Other...>(entt::exclude<Exclude...>);
		}

		template<typename Component, typename Compare, typename Sort = entt::std_sort, typename... Args>
		void sort(Compare compare, Sort algo = Sort{}, Args&&... args)
		{
			m_Registry.sort<Component>(compare, algo, std::forward<Args>(args)...);
		}

		void getSheetsInUse(std::vector<Ref<class Texture2DSheet>>& sheets);

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
		
		void reloadMaps();

	private:
		void DestroyEntityR(Entity entity, Entity parent, bool linkChildren = false);

		void OnPhysicsStart();
		void OnPhysicsStop();

		void BuildScene();
		void DrawScene(EditorCamera& camera);
		void DrawScene(Camera& camera, const glm::mat4& transform);

		void StepPhysics2D(Timestep ts);

		void UpdateScripts(Timestep ts);

		template<typename T>
		void onComponentAdded(Entity entity, T& component);

	private:
		std::string mName;

		entt::registry mRegistry;
		Single<RenderStack> mRenderStack;
		b2World* mPhysicsWorld = nullptr;
		ContactListener* mContactListener = nullptr;

		std::unordered_map<UUID, entt::entity> mEntityMap;
        
		u32 mViewportWidth = 0, mViewportHeight = 0;

		friend Entity;
		friend class ScenePanel;
		friend class ScriptEngine;
		friend class YAMLParser;
	};

}


