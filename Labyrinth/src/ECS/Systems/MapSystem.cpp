#include "ECS/Systems/MapSystem.h"

//Core Engine Includes
#include "Labyrinth.h"
#include "Scene.h"

//ECS Includes
#include "ECS/Entity/Entity.h"
#include "ECS/Components/GameComponents.h"

//Standard Library
#include <fstream>
#include <iostream>
#include <string>

Map::~Map()
{
	Scene::sysTex.destroyTexture(bgTexture);
	Scene::sysTex.destroyTexture(tileTextures);
}

void Map::init(entt::registry& reg)
{

	System::init(reg);

	src.w = src.h = 32;

	dest.w = dest.h = 16;

	tileTextures = Scene::sysTex.loadTexture("assets/textures/worldtextures.png");

	for (int row = 0; row < 40; row++)
	{
		for (int col = 0; col < 50; col++)
		{
			dest.x = col * 16;
			dest.y = row * 16;
			TileComponent::TileID type = static_cast<TileComponent::TileID>(map[row][col]);
		}
	}
}

void Map::loadLevel(int lvl)
{
	std::string lvlPath = "levels/level";
	std::string fileType = ".lvl";

	lvlPath = lvlPath + std::to_string(lvl) + fileType;

	//Clear previous tile entities
	auto view = registry->view<TileComponent>();

	for (auto entity : view)
	{
		registry->destroy(entity);
	}

	std::ifstream tiles(lvlPath);
	if (tiles)
	{
		std::string bgPath = "";
		tiles >> bgPath;
		bgTexture = Scene::sysTex.loadTexture(bgPath.c_str());

		for (int row = 0; row < 40; row++)
		{
			for (int col = 0; col < 50; col++)
			{
				dest.x = col * 16;
				dest.y = row * 16;

				int fileType;
				tiles >> fileType;

				TileComponent::TileID type = static_cast<TileComponent::TileID>(fileType);
				CreateTileEntity(type, false);
			}
		}
	}
	else
	{
		std::cout << "Could not open level file." << std::endl;
	}
}

void Map::CreateTileEntity(TileComponent::TileID typeID, bool collider)
{
	if (registry == nullptr) return;

	Entity newEnt = { registry->create(), registry };
	newEnt.addComponent<TagComponent>(newEnt, "Tile");
	newEnt.addComponent<TileComponent>(newEnt, dest, *tileTextures, typeID);
}