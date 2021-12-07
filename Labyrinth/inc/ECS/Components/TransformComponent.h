#pragma once

#include "SDL.h"

#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/Component.h"

struct TransformComponent : public Component
{
	Vector2D pos;

	int width;
	int height;
	int scale;

	TransformComponent(const TransformComponent&) = default;
	TransformComponent(Entity& entt, const SDL_Rect& rect, int sc = 1) :
		Component(entt), pos({ rect.x, rect.y }), width(rect.w), height(rect.h), scale(sc) {}

};