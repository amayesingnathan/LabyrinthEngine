#include "ECS/Systems/PhysicsEngine.h"

#include "ECS/Components/PhysicsComponent.h"
#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/SpriteComponent.h"

void PhysicsEngine::update()
{
	//Get entities that have required components to update transform
	auto entities = registry->view<VelocityComponent, TransformComponent>();

	for (auto entity : entities)
	{
		//Get components for physics from entity
		auto& trans = entities.get<TransformComponent>(entity);
		const auto& velocity = entities.get<VelocityComponent>(entity);

		//position += scaling factor * velocity * time delta
		trans.pos.x += configuration::FPS * velocity.vel.x * configuration::frameDelay;
		trans.pos.y += configuration::FPS * velocity.vel.y * configuration::frameDelay;
	}

	//Use updated transform to update collider position
	auto colliders = registry->view<ColliderComponent, TransformComponent>();

	for (auto collider : colliders)
	{
		//Get collider component from entity
		auto& box = colliders.get<ColliderComponent>(collider);
		const auto& transform = colliders.get<TransformComponent>(collider);

		//Update collider
		box.collider.x = static_cast<int>(transform.pos.x);
		box.collider.y = static_cast<int>(transform.pos.y);
		box.collider.w = transform.width * transform.scale;
		box.collider.h = transform.height * transform.scale;
	}
}