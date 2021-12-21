#include "ECS/Components/SpriteComponent.h"

#include "Labyrinth.h"
#include "Scene.h"

SpriteComponent::SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated) :
	Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation(suppAnimations::None)
{
	animated = mAnimated;

	animations.emplace(suppAnimations::Idle, Animation(4, 100));
	animations.emplace(suppAnimations::Running, Animation(4, 100));
	animations.emplace(suppAnimations::Falling, Animation(4, 100));

	texture = Scene::sysTex.loadTexture(path);
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

	animations.emplace(suppAnimations::Idle, Animation(4, 100));
	animations.emplace(suppAnimations::Running, Animation(4, 100));
	animations.emplace(suppAnimations::Falling, Animation(4, 100));

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
	Scene::sysTex.destroyTexture(texture);
}

void SpriteComponent::setTex(const char* path)
{
	texture = Scene::sysTex.loadTexture(path);
}

void SpriteComponent::setSubTex(int x, int y, int w, int h)
{
	srcRect = Scene::sysTex.setSubTex(x, y, w, h);
}
