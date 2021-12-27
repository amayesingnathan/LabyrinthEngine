#pragma once

#include "ECS/Systems/System.h"

#include "ECS/Entity/entt.hpp"

class InputManager : public System
{
public:
	void update() override;

private:
	float moveSpeed = 5.0f;
	void updateVelocity(entt::entity entity);
};