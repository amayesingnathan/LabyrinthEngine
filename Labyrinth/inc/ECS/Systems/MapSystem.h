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
	void CreateTileEntity(TileComponent::TileID typeID, bool collider);

private: //Members
	SDL_Rect src, dest;
	SDL_Texture* tileTextures;
	SDL_Texture* bgTexture = nullptr;

	int map[40][50];

};

