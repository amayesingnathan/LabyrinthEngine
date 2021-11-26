#pragma once

#include "VelocityComponent.h"

struct TransformComponent : public Component
{
	Vector2D position;
	Vector2D scale;

	void update() override
	{
		//position += scaling factor * velocity * time delta
		position.x += configuration::FPS * vel.vel.x * configuration::frameDelay;
		position.y += configuration::FPS * vel.vel.y * configuration::frameDelay;
	}	

	TransformComponent() : position(0.0f), scale(1.0f) {}
	TransformComponent(const TransformComponent&) = default;
	template<typename Tpos, typename Tscale>
	TransformComponent(Tpos pos, Tscale scale) : position(pos), scale(scale){}
};