#pragma once

#include "ECS/Entity/Entity.h"
#include "ECS/Components/TileComponent.h"
#include "ECS/Systems/System.h"

#include "config.h"

//XML Parsing Functions
#include "ECS/Systems/XMLParser.h"

#include "SDL.h"

class Map : public System
{
public: //Methods
	~Map();

	void init(entt::registry& reg, const Entity& entt);
	void update() override;

	void loadLevel(int lvl);

	SDL_Texture* getBG() { return bgTexture; };

private:
	void AddTile(int tileID);

public: //Members
	static constexpr int MAP_WIDTH = 100;
	static constexpr int MAP_HEIGHT = 80;
	static constexpr int DISPLAY_WIDTH = 25;
	static constexpr int DISPLAY_HEIGHT = 20;

	/*
		Camera width and height is determined by the size of the map. 
		Take the ratio of map size to display size and subtract one (to account for half a screen width each side)
		and then multiply the result by the screen resolution.
	*/

private:
	static constexpr int CAMERA_WIDTH = ((MAP_WIDTH / DISPLAY_WIDTH) - 1) * configuration::SCREEN_WIDTH;
	static constexpr int CAMERA_HEIGHT = ((MAP_HEIGHT / DISPLAY_HEIGHT) - 1) * configuration::SCREEN_HEIGHT;

	Entity player;

	SDL_Rect src, dest;
	SDL_Texture* tileTextures;
	SDL_Texture* bgTexture = nullptr;

	int map[MAP_HEIGHT][MAP_WIDTH];

	std::map<int, ColliderList> tileColliders;
	int tilesetWidth;
};

