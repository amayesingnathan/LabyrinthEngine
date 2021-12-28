#include "Lpch.h"

#include "ECS/Components/GameComponents.h"

#include "Labyrinth.h"
#include "Scene.h"

#include "ECS/Entity/Entity.h"

SpriteComponent::SpriteComponent(Entity* entt, const char* path, const SDL_Rect& src, bool mAnimated) :
	Component(entt, Types::Sprite), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation()
{
	animated = mAnimated;

	animations.emplace("Idle", Animation(0, 4, 100));
	animations.emplace("Running", Animation(1, 4, 100));
	animations.emplace("Falling", Animation(2, 4, 100));

	texture = Scene::sysTex.loadTexture(path);
	srcRect = src;

	auto& transform = entity->getComponent<TransformComponent>();
	destRect.x = static_cast<int>(transform.pos.x);
	destRect.y = static_cast<int>(transform.pos.y);
	destRect.w = static_cast<int>(round(transform.width * transform.scale.x));
	destRect.h = static_cast<int>(round(transform.height * transform.scale.y));
}

SpriteComponent::SpriteComponent(Entity* entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated) :
	Component(entt, Types::Sprite), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation()
{
	animated = mAnimated;

	animations.emplace("Idle", Animation(0, 4, 100));
	animations.emplace("Running", Animation(1, 4, 100));
	animations.emplace("Falling", Animation(2, 4, 100));

	texture = &tex;
	srcRect = src;

	auto& transform = entity->getComponent<TransformComponent>();
	destRect.x = static_cast<int>(transform.pos.x);
	destRect.y = static_cast<int>(transform.pos.y);
	destRect.w = static_cast<int>(round(transform.width * transform.scale.x));
	destRect.h = static_cast<int>(round(transform.height * transform.scale.y));
}

void SpriteComponent::setTex(const char* path)
{
	texture = Scene::sysTex.loadTexture(path);
}

void SpriteComponent::setSubTex(int x, int y, int w, int h)
{
	srcRect = Scene::sysTex.setSubTex(x, y, w, h);
}
