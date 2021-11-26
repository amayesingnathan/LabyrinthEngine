#pragma once

#include "TextureManager.h"

struct SpriteComponent : public Component
{
	TransformComponent position;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	SpriteComponent() : texture(nullptr), position(), srcRect(), destRect() {}
	SpriteComponent(Entity& entt, const char* path) :
		Component(entt), srcRect(), destRect()
	{
		texture = TextureManager::LoadTexture(path);
		position = entity->getComponent<TransformComponent>();
		srcRect.x = srcRect.y = 0;
		srcRect.w = srcRect.h = 32;
		destRect.w = destRect.h = 64;
	}

	void update() override
	{
		destRect.x = static_cast<int>(position.pos.x);
		destRect.x = static_cast<int>(position.pos.x);
	}

	void draw() override
	{

	}
};