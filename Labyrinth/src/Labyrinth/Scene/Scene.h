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

		void onUpdate(Timestep ts);

	private:
		entt::registry mRegistry;

		friend class Entity;
	};

}