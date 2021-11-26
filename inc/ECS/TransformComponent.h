#pragma once

#include "ECS/Component.h"
#include "Vector2D.h"

struct TransformComponent : public Component
{
	Vector2D position;
	Vector2D scale;

	void update() override;

	TransformComponent() : position(0.0f), scale(1.0f), Component() {}
	TransformComponent(const TransformComponent&) = default;
	template<typename Tpos, typename Tscale>
	TransformComponent(Tpos pos, Tscale scale, Entity& entt) : position(pos), scale(scale), Component(entt){}
};