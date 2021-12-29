#include "Lpch.h"

#include "./GameComponents.h"

#include "ECS/Entity/Entity.h"

TileComponent::TileComponent(Entity* entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex) :
	Component(entt, Types::Tile), destRect(dest), position{dest.x, dest.y}
{
	auto& trans = entity->getComponent<TransformComponent>();
	trans.pos = { dest.x, dest.y };
	trans.width = dest.w;
	trans.height = dest.h;
	sprite = &entity->addComponent<SpriteComponent>(entity, tex, src);

}

void TileComponent::remove()
{
	entity->removeComponent<TransformComponent>();
	entity->removeComponent<SpriteComponent>();
}