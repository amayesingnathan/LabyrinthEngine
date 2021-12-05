#include "ECS/TileComponent.h"

#include "ECS/TransformComponent.h"
#include "ECS/SpriteComponent.h"

TileComponent::TileComponent(Entity& entt, int x, int y, int w, int h, SDL_Texture& tex, TileID id) :
	Component(entt), tileRect{ x, y, w, h }, tileID(id)
{
	SDL_Rect texSrc;
	switch (tileID)
	{
	case TileID::Grass:
		texSrc = { 320, 640, 32, 32};
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

	transform = &entity.addComponent<TransformComponent>(entt, Vector2D(x, y), w, h, 1);
	sprite = &entity.addComponent<SpriteComponent>(entt, tex, texSrc);
}

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

	transform = &entity.addComponent<TransformComponent>(entt, Vector2D(dest.x, dest.y), dest.w, dest.h, 1);
	sprite = &entity.addComponent<SpriteComponent>(entt, tex, texSrc);

}

void TileComponent::remove()
{
	entity.removeComponent<TransformComponent>();
	entity.removeComponent<SpriteComponent>();
}