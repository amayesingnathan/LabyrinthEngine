#pragma once

#include "ECS/Entity/Entity.h"
#include "config.h"


struct Component
{
	Entity entity;

	Component() : entity() {}
	Component(Entity& entt) : entity(entt){}
	virtual ~Component() {}
};