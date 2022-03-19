#include "Lpch.h"

#if 0

#include "./CollisionSystem.h"

#include "Scene.h"

#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

constexpr int Map::MAP_WIDTH;
constexpr int Map::MAP_HEIGHT;
constexpr int Map::DISPLAY_WIDTH;
constexpr int Map::DISPLAY_HEIGHT;

void Collision::init(Scene* scene, Entity* entt)
{
	float widthRatio = static_cast<float>(Map::MAP_WIDTH) / static_cast<float>(Map::DISPLAY_WIDTH);
	float heightRatio = static_cast<float>(Map::MAP_HEIGHT) / static_cast<float>(Map::DISPLAY_HEIGHT);

	System::init(scene);
	player = entt;
	bounds = { configuration::SCREEN_WIDTH * widthRatio, configuration::SCREEN_HEIGHT * heightRatio };
}

void Collision::update()
{
	//Can only check collisions against player so exit if it doesn't have collider.
	if (!player->hasComponent<ColliderComponent>()) return;
	auto& collPlayer = player->getComponent<ColliderComponent>();

	if (!player->hasComponent<TransformComponent>()) return;
	auto& trans = player->getComponent<TransformComponent>();

	bool hasVel = player->hasComponent< VelocityComponent>();

	bool safePos = true;

	//Check that player has not moved out of bounds
	if (trans.pos.x < 0) {
		trans.pos.x = 0;
	}
	if (trans.pos.x + (trans.width * trans.scale.x) > bounds.x) {
		trans.pos.x = bounds.x - (trans.width * trans.scale.x);
	}
	if (trans.pos.y < 0) {
		trans.pos.y = 0; 
	}
	if (trans.pos.y + (trans.height * trans.scale.y) > bounds.y) {
		trans.pos.y = bounds.y - (trans.height * trans.scale.y);

		if (hasVel)
		{
			auto& vel = player->getComponent<VelocityComponent>();
			vel.vel.y = 0;
		}
	}

	//Get entities that have required components to update transform
	auto entities = mScene->mRegistry.view<ColliderComponent>();

	SDL_Rect intersection;

	for (auto entity : entities)
	{
		//Get components for physics from entity
		auto& collOther = entities.get<ColliderComponent>(entity);
		if (player->getID() != entity)
		{
			if (AABB(collPlayer, collOther, &intersection))
			{
				switch (collOther.type)
				{
				case ColliderComponent::Type::Solid:
					safePos = resolveCollision(trans, hasVel);
					break;

				case ColliderComponent::Type::Trigger:
					if (collOther.triggerFunc)
						collOther.triggerFunc();
				}
			}
		}		
	}

	if (safePos)
	{
		trans.lastSafePos = trans.pos;
	}
}

void Collision::addTrigger(Entity& entity, void(*func)())
{
	if (!entity.hasComponent<ColliderComponent>()) {
		entity.addComponent<ColliderComponent>(&entity, SDL_Rect{ 0 }, ColliderComponent::Type::Trigger);
	}

	auto& collider = entity.getComponent<ColliderComponent>();
	if (collider.type == ColliderComponent::Type::Trigger)
	{
		collider.triggerFunc = func;
	}
	else { collider.triggerFunc = nullptr; }
}

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB, SDL_Rect* result)
{
	return SDL_IntersectRect(&recA, &recB, result);
}


bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB, SDL_Rect* result)
{
	return AABB(colA.collider, colB.collider, result);
}

bool Collision::resolveCollision(TransformComponent& trans, bool hasVel)
{
	trans.pos = trans.lastSafePos;

	if (hasVel)
	{
		auto& vel = player->getComponent<VelocityComponent>();
		vel.vel = 0;
	}

	return false;
}

#endif