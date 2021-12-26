#pragma once

#include "ECS/Components/Component.h"
#include "ECS/Components/Vector2D.h"

struct VelocityComponent;
struct SpriteComponent;

struct KeyboardController : public Component
{
	KeyboardController(class Entity* entt);
};