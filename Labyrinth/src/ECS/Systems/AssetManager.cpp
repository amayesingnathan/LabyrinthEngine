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

	TagComponent Tag(newEnt, tag);
	addComponent(*newEnt, &Tag);

	return newEnt;
}

Entity* AssetManager::addPlayer()
{
	Entity* player = createEntity("Player");

	SDL_Rect rect{ configuration::SCREEN_WIDTH / 2, configuration::SCREEN_HEIGHT / 2, 16, 22 };
	int scale = 2;
	std::string playerSpritePath = "assets/PlayerSprite.png";

	//Must add transform component first as sprite component cannot be created without one.
	TransformComponent trans(player, rect, scale);
	addComponent(*player, &trans);

	std::vector<Component*> compList;
	compList.reserve(4);

	VelocityComponent vel(player, 0.f);
	compList.emplace_back(&vel);

	KeyboardController control(player);
	compList.emplace_back(&control);

	ColliderComponent coll(player);
	compList.emplace_back(&coll);

	SpriteComponent sprite(player, playerSpritePath.c_str(), rect, true);
	compList.emplace_back(&sprite);

	addComponents(*player, compList);

	return player;
}


void AssetManager::addComponent(Entity& entity, Component* comp)
{
	switch (comp->derived)
	{
	case Component::Types::Tag:
	{
		auto tag = dynamic_cast<TagComponent*>(comp);
		entity.addComponent<TagComponent>(*tag);
		break;
	}
	case Component::Types::Velocity:
	{
		auto vel = dynamic_cast<VelocityComponent*>(comp);
		entity.addComponent<VelocityComponent>(*vel);
		break;
	}
	case Component::Types::Transform:
	{
		auto trans = dynamic_cast<TransformComponent*>(comp);
		entity.addComponent<TransformComponent>(*trans);
		break;
	}
	case Component::Types::Sprite:
	{
		auto sprite = dynamic_cast<SpriteComponent*>(comp);
		entity.addComponent<SpriteComponent>(*sprite);
		break;
	}
	case Component::Types::Controller:
	{
		auto keys = dynamic_cast<KeyboardController*>(comp);
		entity.addComponent<KeyboardController>(*keys);
		break;
	}
	case Component::Types::Collider:
	{
		auto coll = dynamic_cast<ColliderComponent*>(comp);
		entity.addComponent<ColliderComponent>(*coll);
		break;
	}
	case Component::Types::Tile:
	{
		auto tile = dynamic_cast<TileComponent*>(comp);
		entity.addComponent<TileComponent>(*tile);
		break;
	}
	default:
		break;
	}
}

void AssetManager::addComponents(Entity& entity, std::vector<struct Component*> comps)
{
	for (Component* comp : comps)
	{
		switch (comp->derived)
		{
		case Component::Types::Tag:
		{
			auto tag = dynamic_cast<TagComponent*>(comp);
			entity.addComponent<TagComponent>(*tag);
			break;
		}
		case Component::Types::Velocity:
		{
			auto vel = dynamic_cast<VelocityComponent*>(comp);
			entity.addComponent<VelocityComponent>(*vel);
			break;
		}
		case Component::Types::Transform:
		{
			auto trans = dynamic_cast<TransformComponent*>(comp);
			entity.addComponent<TransformComponent>(*trans);
			break;
		}
		case Component::Types::Sprite:
		{
			auto sprite = dynamic_cast<SpriteComponent*>(comp);
			entity.addComponent<SpriteComponent>(*sprite);
			break;
		}
		case Component::Types::Controller:
		{
			auto keys = dynamic_cast<KeyboardController*>(comp);
			entity.addComponent<KeyboardController>(*keys);
			break;
		}
		case Component::Types::Collider:
		{
			auto coll = dynamic_cast<ColliderComponent*>(comp);
			entity.addComponent<ColliderComponent>(*coll);
			break;
		}
		case Component::Types::Tile:
		{
			auto tile = dynamic_cast<TileComponent*>(comp);
			entity.addComponent<TileComponent>(*tile);
			break;
		}
		default:
			break;
		}
	}
}
