#include "ECS/Components/SpriteComponent.h"

#include "Labyrinth.h"

SpriteComponent::SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated) :
	Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation(suppAnimations::None)
{
	animated = mAnimated;

	animations.emplace(suppAnimations::Idle, Animation(2, 200));
	animations.emplace(suppAnimations::Moving, Animation(12, 100));

	texture = Labyrinth::sysTex.loadTexture(path);
	srcRect = src;

	auto& transform = entity.getComponent<TransformComponent>();
	destRect.x = static_cast<int>(transform.pos.x);
	destRect.y = static_cast<int>(transform.pos.y);
	destRect.w = transform.width * transform.scale;
	destRect.h = transform.height * transform.scale;
}

SpriteComponent::SpriteComponent(Entity& entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated) :
	Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation(suppAnimations::None)
{
	animated = mAnimated;

	animations.emplace(suppAnimations::Idle, Animation(2, 200));
	animations.emplace(suppAnimations::Moving, Animation(12, 100));

	texture = &tex;
	srcRect = src;

	auto& transform = entity.getComponent<TransformComponent>();
	destRect.x = static_cast<int>(transform.pos.x);
	destRect.y = static_cast<int>(transform.pos.y);
	destRect.w = transform.width * transform.scale;
	destRect.h = transform.height * transform.scale;
}

SpriteComponent::~SpriteComponent()
{
	Labyrinth::sysTex.destroyTexture(texture);
}

void SpriteComponent::setTex(const char* path)
{
	texture = Labyrinth::sysTex.loadTexture(path);
}

void SpriteComponent::setSubTex(int x, int y, int w, int h)
{
	srcRect = Labyrinth::sysTex.setSubTex(x, y, w, h);
}
