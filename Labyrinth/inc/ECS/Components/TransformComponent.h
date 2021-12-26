#pragma once

#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/Component.h"

#include "SDL.h"

struct TransformComponent : public Component
{
	Vector2D pos;
	Vector2D lastSafePos;

	int width;
	int height;
	int scale;

	TransformComponent& operator=(const TransformComponent&) = default;

	TransformComponent(class Entity* entt, const SDL_Rect& rect, int sc = 1);

};