#include "ECS/Systems/CollisionSystem.h"

#include "ECS/Entity/Entity.h"

#include "ECS/Components/Vector2D.h"
#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/VelocityComponent.h"
#include "ECS/Components/PhysicsComponent.h"

#include "ECS/Systems/MapSystem.h"

#include <iostream>

constexpr int Map::MAP_WIDTH;
constexpr int Map::MAP_HEIGHT;
constexpr int Map::DISPLAY_WIDTH;
constexpr int Map::DISPLAY_HEIGHT;

void Collision::init(entt::registry& reg, const Entity& entt)
{
	int widthRatio = Map::MAP_WIDTH / Map::DISPLAY_WIDTH;
	int heightRatio = Map::MAP_HEIGHT / Map::DISPLAY_HEIGHT;

	System::init(reg);
	player = entt;
	bounds = { configuration::SCREEN_WIDTH * widthRatio, configuration::SCREEN_HEIGHT * heightRatio };
}

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
