#pragma once

#include "ECS/Component.h"
#include "Vector2D.h"

struct PhysicsComponent;

struct VelocityComponent : public Component
{
	Vector2D vel;

	PhysicsComponent* phys;

	VelocityComponent() : Component(), vel(0.0f), phys(nullptr) {}
	VelocityComponent(const VelocityComponent&) = default;

	template<typename T>
	VelocityComponent(Entity& entt, T velocity, PhysicsComponent* physics) :
		Component(entt), vel(velocity), phys(physics) {}

	void update() override;

};

