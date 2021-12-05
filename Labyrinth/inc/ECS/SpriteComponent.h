#pragma once

#include "TextureManager.h"
#include "ECS/TransformComponent.h"

struct SpriteComponent : public Component
{
	enum class Animations {Idle = 0, Moving = 1};

	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	bool animated;
	int frames;
	int speed;

	Animations animation = Animations::Idle;

	SDL_RendererFlip spriteFlip;

	SpriteComponent(Entity& entt, const char* path, const SDL_Rect& src, bool mAnimated = false) :
		Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE)
	{
		animated = mAnimated;

		texture = TextureManager::LoadTexture(path);
		transform = &entity.getComponent<TransformComponent>();
		srcRect = src;

		destRect.x = static_cast<int>(transform->pos.x);
		destRect.y = static_cast<int>(transform->pos.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	SpriteComponent(Entity& entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated = false) :
		Component(entt), srcRect(), destRect(), frames(0), speed(0), spriteFlip(SDL_FLIP_NONE)
	{
		animated = mAnimated;

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

	void setAnimation(Animations newAnimation) { animation = newAnimation; };

	void update() override
	{
		if (animated)
		{
			if (transform->velocity->vel.isNull())
			{
				animation = Animations::Idle; 
				frames = 2;
				speed = 200;
			}
			else
			{
				animation = Animations::Moving;
				frames = 12;
				speed = 100;
			}
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
			srcRect.y = srcRect.h * static_cast<int>(animation);
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
};