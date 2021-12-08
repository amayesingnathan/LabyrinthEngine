#pragma once

#include "ECS/Components/Component.h"
#include "SDL.h"

struct TagComponent;
struct TransformComponent;

struct ColliderComponent : public Component
{
	SDL_Rect collider;

	ColliderComponent(const ColliderComponent&) = default;
	ColliderComponent(Entity& entt);
};