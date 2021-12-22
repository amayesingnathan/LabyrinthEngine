#pragma once

#include "ECS/Components/Component.h"
#include "ECS/Components/Vector2D.h"

#include "SDL.h"

struct TransformComponent;
struct SpriteComponent;

struct TileComponent : public Component
{
	SDL_Rect destRect;
	Vector2D position;

	TileComponent() = default;

	TileComponent(Entity& entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex);

	void remove();

};