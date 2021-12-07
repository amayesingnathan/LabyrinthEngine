#pragma once

#include "ECS/Systems/System.h"

#include "ECS/Components/VelocityComponent.h"

class InputManager : public System
{
public:
	void update() override;

private:
	void updateVelocity(VelocityComponent& vel);
};