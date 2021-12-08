#pragma once
#include "ECS/Entity/Entity.h"

class System
{
public:
	void init(entt::registry& reg) { registry = &reg; };
	virtual void update() {};

protected:
	entt::registry* registry;
};	