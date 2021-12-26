#pragma once

#include "config.h"

struct Component
{
	enum class Types {Tag = 0, Velocity, Transform, Sprite, Controller, Collider, Tile};

	class Entity* entity;
	Types derived;

	Component(class Entity* entt, Types type);
	virtual ~Component() {}
};
