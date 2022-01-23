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

		void DestroyEntity(Entity entity);

		Entity FindEntity(UUID id);

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity getPrimaryCameraEntity();

	private:
		void CloneNodeComponent(Entity& copyFrom, Entity& copyTo);

		template<typename T>
		void onComponentAdded(Entity entity, T& component);

	private:
		entt::registry mRegistry;
		uint32_t mViewportWidth = 0, mViewportHeight = 0;

		friend Entity;
		friend class SceneSerialiser;
		friend class ScenePanel;
		friend class ScriptEngine;
		friend class YAMLParser;
	};

}


