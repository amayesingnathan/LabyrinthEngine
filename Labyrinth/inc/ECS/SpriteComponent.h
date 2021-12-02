#pragma once

#include "TextureManager.h"

struct SpriteComponent : public Component
{
	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	SpriteComponent(Entity& entt, const char* path, int x = 0, int y = 0, int w = 64, int h = 64) :
		Component(entt), srcRect(), destRect()
	{
		texture = TextureManager::LoadTexture(path);
		transform = &entity.getComponent<TransformComponent>();
		srcRect = TextureManager::setSubTex(x, y, w, h);

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
		destRect.x = static_cast<int>(transform->pos.x);
		destRect.y = static_cast<int>(transform->pos.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void draw() override
	{

	}
};