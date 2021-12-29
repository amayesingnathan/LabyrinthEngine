#pragma once

#include "System.h"

//XML Parsing Functions
#include "ECS/Systems/XMLParser.h"

using RenderLayer = std::vector<struct TileComponent*>;

class Map : public System
{
public: //Methods
	void init(class Scene* scene, class Entity* entt);
	void update() override;
	void clean() override;

	void loadLevel(int lvl, Vector2D spawn);

private:
	struct TileComponent* AddTile(int tileID, const std::pair<int, tilesetData>& set, const Vector2D& pos);

public: //Members
	static constexpr int MAP_WIDTH = 60;
	static constexpr int MAP_HEIGHT = 60;
	static constexpr int DISPLAY_WIDTH = configuration::SCREEN_WIDTH / 32;
	static constexpr int DISPLAY_HEIGHT = configuration::SCREEN_HEIGHT / 32;

	std::vector<RenderLayer> renderLayers;

private:
	/*
		Camera width and height is determined by the size of the map.
		Take the ratio of map size to display size and subtract one (to account for half a screen on each border)
		and then multiply the result by the screen resolution.
	*/
	static constexpr float CAMERA_WIDTH = ((static_cast<float>(MAP_WIDTH) / static_cast<float>(DISPLAY_WIDTH)) - 1) * configuration::SCREEN_WIDTH;
	static constexpr float CAMERA_HEIGHT = ((static_cast<float>(MAP_HEIGHT) / static_cast<float>(DISPLAY_HEIGHT)) - 1) * configuration::SCREEN_HEIGHT;

	class Entity* player;

	SDL_Rect src, dest;
	Vector2D spawn;

	std::vector<Layer> mapLayers;
	std::map<int, tilesetData> tilesets;
	std::map<int, ColliderList> tileColliders;
};

