#pragma once

#include "ECS/Systems/System.h"
#include "ECS/Components/Vector2D.h"
#include "config.h"

#include "ECS/Entity/entt.hpp"

constexpr int configuration::SCREEN_WIDTH;
constexpr int configuration::SCREEN_HEIGHT;

class PhysicsEngine : public System
{
public:
	void update() override;
};