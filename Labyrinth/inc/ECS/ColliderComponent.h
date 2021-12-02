#pragma once

#include "ECS/Component.h"
#include "SDL.h"

struct TagComponent;
struct TransformComponent;

struct ColliderComponent : public Component
{
	SDL_Rect collider;

	TagComponent* tag;
	TransformComponent* transform;

	ColliderComponent() : collider(), tag(nullptr), transform(nullptr) {}
	ColliderComponent(const ColliderComponent&) = default;
	ColliderComponent(Entity& entt);

	void update() override;
};