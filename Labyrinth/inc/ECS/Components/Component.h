#pragma once

#include "config.h"

struct Component
{
	class Entity* entity;

	Component(class Entity* entt);
	virtual ~Component() {}
};