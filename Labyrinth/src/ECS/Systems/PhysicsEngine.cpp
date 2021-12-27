#include "Lpch.h"

#include "ECS/Systems/PhysicsEngine.h"

#include "Scene.h"

#include "ECS/Components/GameComponents.h"

void PhysicsEngine::update()
{
	//Get entities that have required components to update transform
	auto physics = mScene->mRegistry.view<VelocityComponent, TransformComponent>();

	std::for_each(std::execution::par, physics.begin(), physics.end(), [&physics](const auto entity) {
		//Get components for physics from entity
		auto& trans = physics.get<TransformComponent>(entity);
		const auto& velocity = physics.get<VelocityComponent>(entity);

		//position += scaling factor * velocity * time delta
		trans.pos.x += configuration::FPS * velocity.vel.x * configuration::frameDelay;
		trans.pos.y += configuration::FPS * velocity.vel.y * configuration::frameDelay;
	});

	//Use updated transform to update collider position
	auto colliders = mScene->mRegistry.view<ColliderComponent, TransformComponent>();

	std::for_each(std::execution::par, colliders.begin(), colliders.end(), [&colliders](const auto entity){
		//Get collider component from entity
		auto& box = colliders.get<ColliderComponent>(entity);
		const auto& transform = colliders.get<TransformComponent>(entity);

		//Update collider
		box.collider.x = static_cast<int>(transform.pos.x);
		box.collider.y = static_cast<int>(transform.pos.y);
		box.collider.w = transform.width * transform.scale;
		box.collider.h = transform.height * transform.scale;
	});
}
