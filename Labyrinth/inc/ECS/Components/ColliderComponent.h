#pragma once

#include "ECS/Components/Component.h"
#include "SDL_rect.h"

struct ColliderComponent : public Component
{
	SDL_Rect collider;

	ColliderComponent& operator=(const ColliderComponent&) = default;

	ColliderComponent(class Entity* entt);
	ColliderComponent(class Entity* entt, const SDL_Rect& rect);
};