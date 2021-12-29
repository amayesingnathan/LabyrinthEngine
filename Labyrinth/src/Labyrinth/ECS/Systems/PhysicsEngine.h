#pragma once

#include "System.h"
#include "ECS/Components/Vector2D.h"

constexpr int configuration::SCREEN_WIDTH;
constexpr int configuration::SCREEN_HEIGHT;

class PhysicsEngine : public System
{
public:
	void update() override;
};