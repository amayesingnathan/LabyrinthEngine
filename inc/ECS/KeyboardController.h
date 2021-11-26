#pragma once

#include "ECS/Component.h"

struct TransformComponent;

struct KeyboardController : public Component
{
	TransformComponent* transform;

	KeyboardController();

	KeyboardController(Entity entt, TransformComponent* transform);

	void update() override;

};