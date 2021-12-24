#pragma once

#include "ECS/Entity/Entity.h"
#include "ECS/Systems/System.h"

#include "config.h"

//XML Parsing Functions
#include "ECS/Systems/XMLParser.h"

#include "SDL.h"

struct TileComponent;

using RenderLayer = std::vector<TileComponent*>;

class Map : public System
{
public: //Methods
	~Map();

	void init(entt::registry& reg, const Entity& entt);
	void update() override;

	void loadLevel(int lvl);

private:
	TileComponent* AddTile(int tileID, const std::pair<int, tilesetData>& set, const Vector2D& pos);

public: //Members
	static constexpr int MAP_WIDTH = 60;
	static constexpr int MAP_HEIGHT = 60;
	static constexpr int DISPLAY_WIDTH = 25;
	static constexpr int DISPLAY_HEIGHT = 20;

	std::vector<RenderLayer> renderLayers;

private:
	/*
		Camera width and height is determined by the size of the map.
		Take the ratio of map size to display size and subtract one (to account for half a screen on each border)
		and then multiply the result by the screen resolution.
	*/
	static constexpr float CAMERA_WIDTH = ((static_cast<float>(MAP_WIDTH) / static_cast<float>(DISPLAY_WIDTH)) - 1) * configuration::SCREEN_WIDTH;
	static constexpr float CAMERA_HEIGHT = ((static_cast<float>(MAP_HEIGHT) / static_cast<float>(DISPLAY_HEIGHT)) - 1) * configuration::SCREEN_HEIGHT;

	Entity player;

	SDL_Rect src, dest;

	std::vector<Layer> mapLayers;
	std::map<int, tilesetData> tilesets;
	std::map<int, ColliderList> tileColliders;
};

