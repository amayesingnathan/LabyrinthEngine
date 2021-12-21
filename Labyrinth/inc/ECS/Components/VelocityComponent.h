#pragma once
#include "ECS/Components/Component.h"
#include "Vector2D.h"


struct VelocityComponent : public Component
{
	Vector2D vel;

	VelocityComponent(const VelocityComponent&) = default;

	template<typename T>
	VelocityComponent(Entity& entt, T velocity) :
		Component(entt), vel(velocity) {}

};

