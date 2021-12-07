#pragma once

#include "ECS/Systems/System.h"

class PhysicsEngine : public System
{
public:
	void update() override;
};