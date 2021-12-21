#pragma once

#include "SDL.h"
#include "ECS/Components/Component.h"

struct TransformComponent;
struct SpriteComponent;

struct TileComponent : public Component
{
	SDL_Rect tileRect;

	TileComponent() = default;

	TileComponent(Entity& entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex);

	void remove();

};