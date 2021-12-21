#include "ECS/Systems/MapSystem.h"

//Core Engine Includes
#include "Labyrinth.h"
#include "Scene.h"
#include "config.h"

//ECS Includes
#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

//Standard Library
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr int configuration::SCREEN_WIDTH;
constexpr int configuration::SCREEN_HEIGHT;

Map::~Map()
{
	Scene::sysTex.destroyTexture(bgTexture);
	Scene::sysTex.destroyTexture(tileTextures);
}

void Map::init(entt::registry& reg)
{

	System::init(reg);

	src.w = src.h = 32;
	dest.w = configuration::SCREEN_WIDTH / MAP_WIDTH;
	dest.h = configuration::SCREEN_HEIGHT / MAP_HEIGHT;

	tileTextures = Scene::sysTex.loadTexture("assets/textures/worldtextures.png");
}

void Map::loadLevel(int lvl)
{
	std::string lvlPath = "levels/level";
	std::string fileType = ".csv";

	lvlPath = lvlPath + std::to_string(lvl) + fileType;

	//Clear any previous tile entities
	auto view = registry->view<TileComponent>();

	for (auto entity : view)
	{
		registry->destroy(entity);
	}

	std::ifstream tiles(lvlPath);
	if (tiles)
	{
		std::string strWidth;
		std::getline(tiles, strWidth);
		int tilesetWidth = std::stoi(strWidth);
		//bgTexture = Scene::sysTex.loadTexture(bgPath.c_str());

		std::string mapLine;
		std::string mapElement;
		int mapType = -1;
		int tilesetRow = 0;
		int tilesetCol = 0;

		SDL_Rect src{ 0, 0, 32, 32 };

		for (int row = 0; row < MAP_HEIGHT; row++)
		{
			std::getline(tiles, mapLine);
			std::istringstream mapStream(mapLine);
			for (int col = 0; col < MAP_WIDTH; col++)
			{
				std::getline(mapStream, mapElement, ',');
				mapType = std::stoi(mapElement);
				tilesetRow = mapType / tilesetWidth;
				tilesetCol = mapType % tilesetWidth;

				src.x = tilesetCol * 32;
				src.y = tilesetRow * 32;

				dest.x = col * 32;
				dest.y = row * 32;


				bool collider = false;

				AddTile(src, collider);
			}
		}
	}
	else
	{
		std::cout << "Could not open level file." << std::endl;
	}
}

void Map::AddTile(SDL_Rect src, bool collider)
{
	if (registry == nullptr) return;

	Entity newEnt = { registry->create(), registry };
	newEnt.addComponent<TagComponent>(newEnt, "Tile");
	newEnt.addComponent<TileComponent>(newEnt, src, dest, *tileTextures);
	if (collider) newEnt.addComponent<ColliderComponent>(newEnt);

}