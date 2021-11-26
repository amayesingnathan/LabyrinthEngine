#pragma once
#include "ECS\Component.h"
#include "Vector2D.h"
#include "config.h"

struct PhysicsComponent
{

	Vector2D acc;
	bool grav;

	PhysicsComponent() : acc(0, 0), grav(false) {}
	PhysicsComponent(const PhysicsComponent&) = default;
	PhysicsComponent(Vector2D acceleration, bool gravity) : grav(gravity)
	{
		const float g = 9.8f;

		acc = acceleration;
		if (gravity)
		{
			acc.y += g;
		}
	}
	PhysicsComponent(float acc_x, float acc_y, bool gravity) : grav(gravity), acc(Vector2D(acc_x, acc_y))
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