#pragma once

#include "SDL.h"
#include "ECS/Components/Component.h"

struct TransformComponent;
struct SpriteComponent;

struct TileComponent : public Component
{
	enum class TileID {Invalid = -1, Grass = 0, Dirt, Stone};

	SDL_Rect tileRect;
	TileID tileID;

	TileComponent() = default;

	TileComponent(Entity& entt, const SDL_Rect& dest, SDL_Texture& tex, TileID id);

	void remove();

};