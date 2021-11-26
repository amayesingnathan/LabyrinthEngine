#pragma once

#include "ECS/Entity.h"
#include "config.h"


struct Component
{
	Entity* entity;

	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}

	Component() : entity() {}
	Component(Entity& entt) : entity(&entt){}
	virtual ~Component() {}
};