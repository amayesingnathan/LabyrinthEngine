#include "ECS/VelocityComponent.h"
#include "ECS/PhysicsComponent.h"

void VelocityComponent::update()
{
	//velocity = acceleration * time delta
	vel.x += (phys->acc.x * configuration::frameDelay);
	vel.y += (phys->acc.y * configuration::frameDelay);
}