#include "ECS/TileComponent.h"

#include "ECS/TransformComponent.h"
#include "ECS/SpriteComponent.h"

TileComponent::TileComponent(Entity& entt, int x, int y, int w, int h, TileID id) : 
	Component(entt), tileRect{ x, y, w, h }, tileID(id)
{
	transform = &entity.getComponent<TransformComponent>();
	sprite = &entity.getComponent<SpriteComponent>();

	switch (tileID)
	{
	case TileID::Grass:
		break;

	case TileID::Dirt:
		break;

	case TileID::Stone:
		break;

	default:
		break;
	}
}