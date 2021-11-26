#pragma once
#include "ECS\TransformComponent.h"
#include "ECS\Entity.h"
#include "ECS\Component.h"

struct SpriteComponent : public Component
{
	TransformComponent* position;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	SpriteComponent() = default;
	SpriteComponent(const char* path, Entity& entt, const entt::registry& reg)
	{
		Component(entt, reg);
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{
		position = &entity.getComponent<TransformComponent>();
	}

	void update() override
	{

	}

	void draw() override
	{

	}
};