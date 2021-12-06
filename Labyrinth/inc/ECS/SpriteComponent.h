#pragma once

#include "TextureManager.h"
#include "ECS/TransformComponent.h"
#include "ECS/Animation.h"

struct SpriteComponent : public Component
{
	enum class suppAnimations { None = -1, Idle = 0, Moving = 1 };

	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	bool animated;
	int frames;
	int speed;

	suppAnimations currAnimation;
	std::map<suppAnimations, Animation> animations;

	SDL_RendererFlip spriteFlip;

	SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated = false) :
		Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation(suppAnimations::None)
	{
		animated = mAnimated;
		
		animations.emplace(suppAnimations::Idle, Animation(2, 200));
		animations.emplace(suppAnimations::Moving, Animation(12, 100));

		texture = TextureManager::LoadTexture(path);
		transform = &entity.getComponent<TransformComponent>();
		srcRect = src;

		destRect.x = static_cast<int>(transform->pos.x);
		destRect.y = static_cast<int>(transform->pos.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	SpriteComponent(Entity& entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated = false) :
		Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE), currAnimation(suppAnimations::None)
	{
		animated = mAnimated;

		animations.emplace(suppAnimations::Idle, Animation(2, 200));
		animations.emplace(suppAnimations::Moving, Animation(12, 100));

		texture = &tex;
		transform = &entity.getComponent<TransformComponent>();
		srcRect = src;

		destRect.x = static_cast<int>(transform->pos.x);
		destRect.y = static_cast<int>(transform->pos.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	~SpriteComponent() override
	{
		TextureManager::DestroyTexture(texture);
	}

	void setTex(const char* path)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void setSubTex(int x, int y, int w, int h)
	{
		srcRect = TextureManager::setSubTex(x, y, w, h);
	}

	void update() override
	{
		if (animated)
		{
			if (transform->velocity->vel.isNull())
			{
				play(suppAnimations::Idle);
			}
			else
			{
				play(suppAnimations::Moving);
			}
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
			srcRect.y = srcRect.h * static_cast<int>(currAnimation);
		}

		destRect.x = static_cast<int>(transform->pos.x);
		destRect.y = static_cast<int>(transform->pos.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}

	void addAnimation(suppAnimations anim, int f, int s)
	{
		animations.emplace(anim, Animation(f, s));
	}

	void play(suppAnimations anim)
	{
		if (!animations.count(anim)) return; //Return if animation has not been added

		currAnimation = anim;
		frames = animations[anim].frames;
		speed = animations[anim].speed;
	}

};