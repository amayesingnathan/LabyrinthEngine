#include "ECS/Components/TileComponent.h"

#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/SpriteComponent.h"

TileComponent::TileComponent(Entity& entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex) :
	Component(entt), destRect(dest), position{dest.x, dest.y}
{
	entity.addComponent<TransformComponent>(entity, dest, 1);
	sprite = &entity.addComponent<SpriteComponent>(entity, tex, src);

}

void TileComponent::remove()
{
	entity.removeComponent<TransformComponent>();
	entity.removeComponent<SpriteComponent>();
}