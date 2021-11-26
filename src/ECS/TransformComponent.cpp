#include "ECS/TransformComponent.h"

#include "ECS/VelocityComponent.h"
#include "config.h"

void TransformComponent::update()
{
	auto& velocity = entity->getComponent<VelocityComponent>();
	//position += scaling factor * velocity * time delta
	position.x += configuration::FPS * velocity.vel.x * configuration::frameDelay;
	position.y += configuration::FPS * velocity.vel.y * configuration::frameDelay;
}
