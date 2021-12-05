#pragma once

#include "SDL.h"
#include "ECS/Component.h"

struct TransformComponent;
struct SpriteComponent;

struct TileComponent : public Component
{
	enum class TileID {Invalid = -1, Grass = 0, Dirt, Stone};

	TransformComponent* transform;
	SpriteComponent* sprite;

	SDL_Rect tileRect;
	TileID tileID;

	TileComponent() = default;

	TileComponent(Entity& entt, int x, int y, int w, int h, SDL_Texture& tex, TileID id);
	TileComponent(Entity& entt, const SDL_Rect& dest, SDL_Texture& tex, TileID id);

	void remove();

};