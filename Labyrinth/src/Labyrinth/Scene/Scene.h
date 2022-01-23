#pragma once

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

		Entity CreateEntity(const std::string& name, const Entity& parent);
		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity getPrimaryCameraEntity();

	private:
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


