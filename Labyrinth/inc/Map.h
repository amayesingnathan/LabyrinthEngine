#pragma once

#include "SDL.h"
#include "ECS/entt.hpp"
#include "ECS/TileComponent.h"

class Map
{
public: //Methods

	Map();
	~Map();

	void init(entt::registry* eng);

	void loadLevel(int lvl);
	void drawMap();

	void CreateTileEntity(TileComponent::TileID typeID, bool collider);

private: //Members
	entt::registry* registry;
	SDL_Rect src, dest;
	SDL_Texture* textures;

	int map[40][50];

};

