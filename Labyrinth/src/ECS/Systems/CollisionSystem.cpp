#include "ECS/Systems/CollisionSystem.h"

#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/VelocityComponent.h"

#include "ECS/Entity/Entity.h"

#include <iostream>

void Collision::update()
{
	//Can only check collisions against player so exit if it doesn't have collider.
	if (!player.hasComponent<ColliderComponent>()) return;
	auto& collPlayer = player.getComponent<ColliderComponent>();

	bool hasVel = player.hasComponent<VelocityComponent>();

	//Get entities that have required components to update transform
	auto entities = registry->view<ColliderComponent>();

	for (auto entity : entities)
	{
		//Get components for physics from entity
		auto& collOther = entities.get<ColliderComponent>(entity);
		if (player.getID() != entity)
		{
			if (AABB(collPlayer, collOther) && hasVel)
			{
				std::cout << "Player hit something!" << std::endl;
				auto& vel = player.getComponent<VelocityComponent>();
				vel.vel *= -1;
			}
		}		
	}
}

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB)
{
	return SDL_HasIntersection(&recA, &recB);
}


bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB)
{
	return AABB(colA.collider, colB.collider);
}