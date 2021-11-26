#pragma once
#include "PhysicsComponent.h"

struct VelocityComponent : public Component
{
	Vector2D vel;
	void update() override
	{
		auto& physics = entity.getComponent<PhysicsComponent>();
		//velocity = acceleration * time delta
		vel.x += (physics.acc.x * configuration::frameDelay);
		vel.y += (physics.acc.y * configuration::frameDelay);
	}
};