#pragma once
#include "ECS/Entity/Entity.h"
#include "ECS/Components/Component.h"

#include "Vector2D.h"

struct VelocityComponent : public Component
{
	Vector2D vel;

	VelocityComponent& operator=(const VelocityComponent&) = default;

	template<typename T>
	VelocityComponent(class Entity* entt, T velocity) :
		Component(entt, Types::Velocity), vel(velocity) {}
};

