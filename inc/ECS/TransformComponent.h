#pragma once

#include "ECS/Component.h"
#include "Vector2D.h"

struct VelocityComponent;

struct TransformComponent : public Component
{
	Vector2D pos;

	VelocityComponent* velocity;

	void update() override;

	TransformComponent() : Component(), pos(0.0f), velocity() {}
	TransformComponent(const TransformComponent&) = default;
	template<typename T>
	TransformComponent(Entity& entt, T position, VelocityComponent* vel) :
		Component(entt), pos(position), velocity(vel)
	{}
};