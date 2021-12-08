#pragma once

#include "ECS/Components/Component.h"
#include "SDL_rect.h"

struct TagComponent;
struct TransformComponent;

struct ColliderComponent : public Component
{
	SDL_Rect collider;

	ColliderComponent(const ColliderComponent&) = default;
	ColliderComponent(Entity& entt);
	ColliderComponent(Entity& entt, const SDL_Rect& rect) : Component(entt), collider(rect) {}
};