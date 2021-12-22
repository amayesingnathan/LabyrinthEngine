#pragma once

#include "ECS/Systems/System.h"

struct VelocityComponent;
struct PhysicsComponent;

class InputManager : public System
{
public:
	void update() override;

private:
	float moveSpeed = 10.0f;
	void updateVelocity(VelocityComponent& vel);
	void updatePhysics(PhysicsComponent& phys);
};