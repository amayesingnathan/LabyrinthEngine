#pragma once

#include "SDL.h"
#include "ECS/Entity/entt.hpp"
#include "ECS/Components/TileComponent.h"
#include "ECS/Systems/System.h"

class Map : public System
{
public: //Methods
	~Map();

	void init(entt::registry& reg);
	void update() override {}

	void loadLevel(int lvl);

	SDL_Texture* getBG() { return bgTexture; };

private:
	void AddTile(SDL_Rect src, bool collider);

private: //Members
	static constexpr int MAP_WIDTH = 25;
	static constexpr int MAP_HEIGHT = 20;

	SDL_Rect src, dest;
	SDL_Texture* tileTextures;
	SDL_Texture* bgTexture = nullptr;

	int map[MAP_HEIGHT][MAP_WIDTH];

};

