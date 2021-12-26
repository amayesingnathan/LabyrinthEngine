#include "ECS\Systems\AssetManager.h"

#include "Scene.h"
#include "config.h"

#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

AssetManager::~AssetManager()
{
	for (auto entity : mEntities)
	{
		mScene->mRegistry.destroy(entity->getID());
		delete entity;
		entity = nullptr;
	}
}

Entity* AssetManager::createEntity(const std::string& tag)
{
	Entity* newEnt = new Entity{ mScene->mRegistry.create(), mScene };
	mEntities.push_back(newEnt);
	newEnt->addComponent<TagComponent>(newEnt, tag);
	return newEnt;
}

Entity* AssetManager::addPlayer()
{
	Entity* player = createEntity("Player");

	SDL_Rect rect{ configuration::SCREEN_WIDTH / 2, configuration::SCREEN_HEIGHT / 2, 16, 22 };
	int scale = 2;


	player->addComponent<VelocityComponent>(player, 0.0f);
	player->addComponent<TransformComponent>(player, rect, scale);
	player->addComponent<KeyboardController>(player);
	player->addComponent<ColliderComponent>(player);
	std::string playerSpritePath = "assets/PlayerSprite.png";
	player->addComponent<SpriteComponent>(player, playerSpritePath.c_str(), rect, true);

	return player;
}
