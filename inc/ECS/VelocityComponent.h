#pragma once

#include "ECS/Component.h"
#include "Vector2D.h"

struct VelocityComponent : public Component
{
	Vector2D vel;

	VelocityComponent() : vel(0.0f), Component() {}
	VelocityComponent(const VelocityComponent&) = default;

	template<typename T>
	VelocityComponent(T velocity, Entity& entt) : vel(velocity), Component(entt) {}

	void update() override;
};