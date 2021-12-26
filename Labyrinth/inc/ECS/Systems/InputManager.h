#pragma once

#include "ECS/Systems/System.h"

class InputManager : public System
{
public:
	void update() override;

private:
	float moveSpeed = 5.0f;
	void updateVelocity(struct VelocityComponent& vel);
};