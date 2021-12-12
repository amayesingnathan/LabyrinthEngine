#include "ECS/Systems/CollisionSystem.h"

#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/VelocityComponent.h"

#include "ECS/Entity/Entity.h"

#include <iostream>

void Collision::update()
{
	//Can only check collisions against player so exit if it doesn't have collider.
	if (!player.hasComponent<ColliderComponent>()) return;
	auto& collPlayer = player.getComponent<ColliderComponent>();

	if (!player.hasComponent<TransformComponent>()) return;
	auto& trans = player.getComponent<TransformComponent>();

	bool hasVel = player.hasComponent< VelocityComponent>();

	bool safePos = true;

	//Check that player has not moved out of bounds
	if (trans.pos.x < 0) {
		trans.pos.x = 0;
	}
	if (trans.pos.x + (trans.width * trans.scale) > bounds.x) {
		trans.pos.x = bounds.x - (trans.width * trans.scale);
	}
	if (trans.pos.y < 0) {
		trans.pos.y = 0; 
	}
	if (trans.pos.y + (trans.height * trans.scale) > bounds.y) {
		trans.pos.y = bounds.y - (trans.height * trans.scale);

		if (hasVel)
		{
			auto& vel = player.getComponent<VelocityComponent>();
			vel.vel.y = 0;
		}

		if (player.hasComponent<PhysicsComponent>())
		{
			auto& phys = player.getComponent<PhysicsComponent>();
			phys.grounded = true;
			phys.jumpStart = 0;
		}
	}

	//Get entities that have required components to update transform
	auto entities = registry->view<ColliderComponent>();

	SDL_Rect intersection;

	for (auto entity : entities)
	{
		//Get components for physics from entity
		auto& collOther = entities.get<ColliderComponent>(entity);
		if (player.getID() != entity)
		{
			if (AABB(collPlayer, collOther, &intersection))
			{
				std::cout << "Player hit something!" << std::endl;
				trans.pos = trans.lastSafePos;

				if (hasVel)
				{
					auto& vel = player.getComponent<VelocityComponent>();
					vel.vel = 0;
				}

				safePos = false;
			}
		}		
	}

	if (safePos)
	{
		trans.lastSafePos = trans.pos;
	}
}

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB, SDL_Rect* result)
{
	return SDL_IntersectRect(&recA, &recB, result);
}


bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB, SDL_Rect* result)
{
	return AABB(colA.collider, colB.collider, result);
}