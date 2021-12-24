#pragma once

#include "ECS/Components/Component.h"
#include "ECS/Components/Vector2D.h"

#include "SDL.h"

struct SpriteComponent;

struct TileComponent : public Component
{
	SDL_Rect destRect;
	Vector2D position;
	SpriteComponent* sprite;

	TileComponent() = default;

	TileComponent(Entity& entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex);

	void remove();

};