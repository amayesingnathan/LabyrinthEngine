#pragma once

#include "TextureManager.h"

struct SpriteComponent : public Component
{
	TransformComponent* position;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	SpriteComponent() = default;
	SpriteComponent(const char* path, Entity& entt) : 
		Component(entt)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{
		position = &entity->getComponent<TransformComponent>();
	}

	void update() override
	{

	}

	void draw() override
	{

	}
};