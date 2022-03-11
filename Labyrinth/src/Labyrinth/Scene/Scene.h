#pragma once

#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Renderer/EditorCamera.h"

#include "entt.hpp"

namespace Labyrinth {

	class Entity;

	class Scene : public AllowRefFromThis<Scene>
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, Entity& parent);

		Entity CreateEntityWithID(const UUID& id, const std::string& name);
		Entity CreateEntityWithID(const UUID& id, const std::string& name, Entity& parent);

		Entity CloneEntity(Entity& copy);
		Entity CloneChild(Entity& copy, Entity& newParent);

		void DestroyEntity(Entity& entity);

		Entity FindEntity(UUID id);

		template<typename Component, typename... Other, typename... Exclude>
		entt::basic_view<entt::entity, entt::get_t<Component, Other...>, entt::exclude_t<Exclude...>> view(entt::exclude_t<Exclude...> = {})
		{
			return mRegistry.view<Component, Other...>(entt::exclude<Exclude...>);
		}

		void getSheetsInUse(std::vector<Ref<class Texture2DSheet>>& sheets);

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity getPrimaryCameraEntity();

	private:
		void DestroyEntityR(Entity& entity, Entity& parent);

		template<typename T>
		void onComponentAdded(Entity entity, T& component);

	private:
		entt::registry mRegistry;
		uint32_t mViewportWidth = 0, mViewportHeight = 0;

		friend Entity;
		friend class ScenePanel;
		friend class ScriptEngine;
		friend class YAMLParser;
	};

}


