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
#include <sstream>
#include <string>

Map::~Map()
{
	Scene::sysTex.destroyTexture(bgTexture);
	Scene::sysTex.destroyTexture(tileTextures);
}

void Map::init(entt::registry& reg, const Entity& entt)
{

	System::init(reg);
	player = entt;

	src.w = src.h = 32;
	dest.w = configuration::SCREEN_WIDTH / DISPLAY_WIDTH;
	dest.h = configuration::SCREEN_HEIGHT / DISPLAY_HEIGHT;

	Scene::camera.w = CAMERA_WIDTH;
	Scene::camera.h = CAMERA_HEIGHT;

	tileTextures = Scene::sysTex.loadTexture("assets/textures/worldtextures.png");
	tileColliders = XMLParser::getTileColliders("worldtextures");
}

void Map::update()
{
	const auto& transform = player.getComponent<TransformComponent>();

	Scene::camera.x = transform.pos.x - (configuration::SCREEN_WIDTH / 2);
	Scene::camera.y = transform.pos.y - (configuration::SCREEN_HEIGHT / 2);

	if (Scene::camera.x < 0) Scene::camera.x = 0;
	if (Scene::camera.y < 0) Scene::camera.y = 0;
	if (Scene::camera.x > Scene::camera.w) Scene::camera.x = Scene::camera.w;
	if (Scene::camera.y > Scene::camera.h) Scene::camera.y = Scene::camera.h;

	auto view = registry->view<TileComponent>();

	for (auto tiles : view)
	{
		auto& tile = registry->get<TileComponent>(tiles);
		tile.destRect.x = tile.position.x - Scene::camera.x;
		tile.destRect.y = tile.position.y - Scene::camera.y;
	}

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

		for (int row = 0; row < MAP_HEIGHT; row++)
		{
			std::getline(tiles, mapLine);
			std::istringstream mapStream(mapLine);
			for (int col = 0; col < MAP_WIDTH; col++)
			{
				std::getline(mapStream, mapElement, ',');\
				mapType = std::stoi(mapElement);
				tilesetRow = mapType / tilesetWidth;
				tilesetCol = mapType % tilesetWidth;

				src.x = tilesetCol * 32;
				src.y = tilesetRow * 32;

				dest.x = col * 32;
				dest.y = row * 32;

				AddTile(mapType);
			}
		}
	}
	else
	{
		std::cout << "Could not open level file." << std::endl;
	}
}

void Map::AddTile(int tileID)
{
	if (registry == nullptr) return;

	Entity newEnt = { registry->create(), registry };
	newEnt.addComponent<TagComponent>(newEnt, "Tile");
	newEnt.addComponent<TileComponent>(newEnt, src, dest, *tileTextures);

	if (tileColliders.count(tileID))
	{
		for (auto collider : tileColliders[tileID])
		{
			Entity newCollider = { registry->create(), registry };
			SDL_Rect colliderRect{ dest.x + collider.x, dest.y + collider.y, collider.w, collider.h };
			newCollider.addComponent<ColliderComponent>(newEnt, colliderRect);
		}
	}
}