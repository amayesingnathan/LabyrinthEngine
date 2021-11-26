#pragma once
#include "Vector2D.h"
#include "ECS/Component.h"

struct PhysicsComponent : public Component
{

	Vector2D acc;
	bool grav;

	PhysicsComponent() : acc(0.0f), grav(false), Component() {}
	PhysicsComponent(const PhysicsComponent&) = default;
	PhysicsComponent(Vector2D acceleration, bool gravity, Entity& entt) : grav(gravity), Component(entt)
	{
		const float g = 9.8f;

		acc = acceleration;
		if (gravity)
		{
			acc.y += g;
		}
	}
	PhysicsComponent(float acc_x, float acc_y, bool gravity, Entity& entt) : grav(gravity), acc(Vector2D(acc_x, acc_y)), Component(entt)
	{
		const float g = 9.8f;

		if (gravity)
		{
			acc.y += g;
		}
	}

	void update()
	{

	}
};