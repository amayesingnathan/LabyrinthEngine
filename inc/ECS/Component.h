#pragma once
#include "entt.hpp"

struct Component
{
	const Entity& entity;
	const entt::registry* ecs;

	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}

	Component() : entity(entt::null), ecs(nullptr) {}
	Component(const Entity& entt, const entt::registry& reg) : entity(entt), ecs(&reg) {}
	virtual ~Component() {}
};