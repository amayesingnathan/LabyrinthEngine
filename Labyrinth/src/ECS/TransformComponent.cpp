#include "ECS/TransformComponent.h"

#include "config.h"

void TransformComponent::update()
{
	//position += scaling factor * velocity * time delta
	pos.x += configuration::FPS * velocity->vel.x * configuration::frameDelay;
	pos.y += configuration::FPS * velocity->vel.y * configuration::frameDelay;
}
