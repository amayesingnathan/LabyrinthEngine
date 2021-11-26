#include "ECS/VelocityComponent.h"
#include "ECS/PhysicsComponent.h"

void VelocityComponent::update()
{
	auto& physics = entity->getComponent<PhysicsComponent>();
	//velocity = acceleration * time delta
	vel.x += (physics.acc.x * configuration::frameDelay);
	vel.y += (physics.acc.y * configuration::frameDelay);
}