#include "Lpch.h"

#include "ECS\Systems\AssetManager.h"

#include "Scene.h"

#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

void AssetManager::clean()
{
	for (auto entity : mEntities)
	{
		mScene->mRegistry.destroy(entity->getID());
		delete entity;
		entity = nullptr;
	}
	std::cout << "Asset Manager cleaned\n";
}

Entity* AssetManager::createEntity(const std::string& tag)
{
	Entity* newEnt = new Entity{ mScene->mRegistry.create(), mScene };
	mEntities.push_back(newEnt);

	TagComponent Tag(newEnt, tag);
	addComponent(*newEnt, &Tag);

	TransformComponent trans(newEnt, { 0 }, 1);
	if (tag == "Player")
	{
		trans.pos = { configuration::SCREEN_WIDTH / 2, configuration::SCREEN_HEIGHT / 2 };
		trans.width = 16;
		trans.height = 22;
		trans.scale = 2;
	}
	addComponent(*newEnt, &trans);

	return newEnt;
}

void AssetManager::destroyEntity(Entity& entity)
{
	mScene->mRegistry.destroy(entity);
}

Entity* AssetManager::addPlayer()
{
	Entity* player = createEntity("Player");

	SDL_Rect rect{ configuration::SCREEN_WIDTH / 2, configuration::SCREEN_HEIGHT / 2, 16, 22 };
	int scale = 2;
	std::string playerSpritePath = "assets/PlayerSprite.png";

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
	case Component::Types::Script:
	{
		auto script = dynamic_cast<ScriptComponent*>(comp);
		entity.addComponent<ScriptComponent>(*script);
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

template<typename T>
void AssetManager::OnComponentAdded(Entity entity, T& component)
{
	static_assert(false);
}

template<>
void AssetManager::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template<>
void AssetManager::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& component)
{
}

template<>
void AssetManager::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template<>
void AssetManager::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
{
}