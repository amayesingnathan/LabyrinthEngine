#pragma once

#include "ECS/PhysicsComponent.h"
#include "ECS/Component.h"
#include "Vector2D.h"


struct VelocityComponent : public Component
{
	Vector2D vel;

	PhysicsComponent* phys;

	VelocityComponent() : Component(), vel(0.0f), phys(nullptr) {}
	VelocityComponent(const VelocityComponent&) = default;

	template<typename T>
	VelocityComponent(Entity& entt, T velocity) :
		Component(entt), vel(velocity) {
		phys = &entity.addComponent<PhysicsComponent>(entity, 0.0f, false);
	}

	void update() override;

};

