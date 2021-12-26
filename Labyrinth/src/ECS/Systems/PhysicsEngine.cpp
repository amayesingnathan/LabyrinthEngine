#include "ECS/Systems/PhysicsEngine.h"

#include "Scene.h"

#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/SpriteComponent.h"

void PhysicsEngine::update()
{
	//Get entities that have required components to update transform
	auto entities = mScene->mRegistry.view<VelocityComponent, TransformComponent>();

	for (auto entity : entities)
	{
		//Get components for physics from entity
		auto& trans = mScene->mRegistry.get<TransformComponent>(entity);
		const auto& velocity = mScene->mRegistry.get<VelocityComponent>(entity);

		//position += scaling factor * velocity * time delta
		trans.pos.x += configuration::FPS * velocity.vel.x * configuration::frameDelay;
		trans.pos.y += configuration::FPS * velocity.vel.y * configuration::frameDelay;
	}

	//Use updated transform to update collider position
	auto colliders = mScene->mRegistry.view<ColliderComponent, TransformComponent>();

	for (auto collider : colliders)
	{
		//Get collider component from entity
		auto& box = mScene->mRegistry.get<ColliderComponent>(collider);
		const auto& transform = mScene->mRegistry.get<TransformComponent>(collider);

		//Update collider
		box.collider.x = static_cast<int>(transform.pos.x);
		box.collider.y = static_cast<int>(transform.pos.y);
		box.collider.w = transform.width * transform.scale;
		box.collider.h = transform.height * transform.scale;
	}
}