#pragma once

#include "ECS/VelocityComponent.h"
#include "ECS/Component.h"
#include "Vector2D.h"

struct TransformComponent : public Component
{
	Vector2D pos;

	VelocityComponent* velocity;

	int width;
	int height;
	int scale;

	void update() override;

	TransformComponent() : Component(), pos(0.0f), velocity(nullptr), width(32), height(32), scale(1) {}
	TransformComponent(const TransformComponent&) = default;
	template<typename T>
	TransformComponent(Entity& entt, const T position, int w = 32, int h = 32, int sc = 1) :
		Component(entt), pos(position), width(w), height(h), scale(sc)
	{
		velocity = &entity.addComponent<VelocityComponent>(entity, 0.0f);
	}

};