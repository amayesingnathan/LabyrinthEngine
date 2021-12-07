#include "ECS/Components/TileComponent.h"

#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/SpriteComponent.h"

TileComponent::TileComponent(Entity& entt, const SDL_Rect& dest, SDL_Texture& tex, TileID id) :
	Component(entt), tileRect(dest), tileID(id)
{
	SDL_Rect texSrc;
	switch (tileID)
	{
	case TileID::Grass:
		texSrc = { 320, 640, 32, 32 };
		break;

	case TileID::Dirt:
		texSrc = { 0, 576, 32, 32 };
		break;

	case TileID::Stone:
		texSrc = { 96, 896, 32, 32 };
		break;

	default:
		break;
	}

	entity.addComponent<TransformComponent>(entt, dest, 1);
	entity.addComponent<SpriteComponent>(entt, tex, texSrc);

}

void TileComponent::remove()
{
	entity.removeComponent<TransformComponent>();
	entity.removeComponent<SpriteComponent>();
}