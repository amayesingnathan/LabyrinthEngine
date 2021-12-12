#pragma once

#include "ECS/Systems/System.h"

#include "ECS/Components/VelocityComponent.h"

class InputManager : public System
{
public:
	void update() override;

private:
	float moveSpeed = 3.5f;
	void updateVelocity(VelocityComponent& vel);
	void updatePhysics(PhysicsComponent& phys);
};