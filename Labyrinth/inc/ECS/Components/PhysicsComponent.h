#pragma once
#include "Vector2D.h"
#include "ECS/Components/Component.h"

struct PhysicsComponent : public Component
{
	Vector2D acc;
	bool grav;
	bool grounded;
	int jumpStart;

	PhysicsComponent() : Component(), acc(0.0f), grav(false), grounded(false), jumpStart(0) {}
	PhysicsComponent(const PhysicsComponent&) = default;
	PhysicsComponent(Entity& entt, Vector2D acceleration, bool gravity) : Component(entt), acc(acceleration), grav(gravity), grounded(false), jumpStart(0)
	{}
	PhysicsComponent(Entity& entt, float acc_x, float acc_y, bool gravity) : Component(entt), acc(Vector2D(acc_x, acc_y)), grav(gravity), grounded(false), jumpStart(0)
	{}

};