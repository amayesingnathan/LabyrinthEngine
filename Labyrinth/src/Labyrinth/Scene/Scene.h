#pragma once

#include "Labyrinth/Core/Timestep.h"

#include "entt.hpp"

namespace Labyrinth {

	class Scene
	{
	public:
		Scene();
		~Scene();

		class Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void onUpdate(Timestep ts);
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry mRegistry;
		uint32_t mViewportWidth = 0, mViewportHeight = 0;

		friend class Entity;
		friend class ScenePanel;
	};

}