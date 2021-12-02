#pragma once

#include "ECS/Component.h"
#include "ECS/Vector2D.h"

struct VelocityComponent;

struct KeyboardController : public Component
{
	VelocityComponent* velocity;

	Vector2D lerpTime;

	KeyboardController();
	KeyboardController(Entity entt);

	void update() override;
	void updateVelocity(VelocityComponent& vel);

};