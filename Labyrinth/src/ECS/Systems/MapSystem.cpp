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
	for (auto tex : tilesets)
	{
		Scene::sysTex.destroyTexture(tex.second.tilesetTex);
	}
}

void Map::init(entt::registry& reg, const Entity& entt)
{
	System::init(reg);
	player = entt;

	dest.w = configuration::SCREEN_WIDTH / DISPLAY_WIDTH;
	dest.h = configuration::SCREEN_HEIGHT / DISPLAY_HEIGHT;

	Scene::camera.w = static_cast<int>(round(CAMERA_WIDTH));
	Scene::camera.h = static_cast<int>(round(CAMERA_HEIGHT));
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
	std::string lvlPath = "levels/level" + std::to_string(lvl) + "/level";
	std::string fileType = ".tmx";

	lvlPath = lvlPath + std::to_string(lvl) + fileType;

	//Clear any previous tile entities
	auto view = registry->view<TileComponent>();

	for (auto entity : view)
	{
		registry->destroy(entity);
	}

	XMLParser::openLevel(lvl, mapLayers, tilesets, tileColliders);

	renderLayers.reserve(mapLayers.capacity());

	for (auto layer : mapLayers)
	{
		RenderLayer currLayer;
		int layerTileCount = layer.capacity() * layer.begin()->capacity();
		currLayer.reserve(layerTileCount);

		int colCount = 0;
		for (auto col : layer)
		{
			int rowCount = 0;
			for (auto tile : col)
			{
				//Finds the last tileset with a first ID value that is less than or equal to the current tile ID
				auto it = std::find_if(tilesets.rbegin(), tilesets.rend(),
					[tile](const auto& set) {
						return set.first <= tile;
					});
				if (it != tilesets.rend())
				{
					currLayer.emplace_back(AddTile(tile, *it, { rowCount, colCount }));
				}
				else
				{
					currLayer.emplace_back(AddTile(tile, *tilesets.begin(), { rowCount, colCount }));
				}
				rowCount++;
			}
			colCount++;
		}
		renderLayers.emplace_back(currLayer);
		currLayer.clear();
	}
}

TileComponent* Map::AddTile(int tileID, const std::pair<int, tilesetData>& tileset, const Vector2D& pos)
{
	if (registry == nullptr) return nullptr;
	if (tileset.second.tilesetTex == NULL) return nullptr;

	int tilesetRow = (tileID - tileset.first) / tileset.second.tilesetWidth;
	int tilesetCol = (tileID - tileset.first) % tileset.second.tilesetWidth;

	src.w = tileset.second.tileWidth;
	src.h = tileset.second.tileHeight;
	src.x = tilesetCol * src.w;
	src.y = tilesetRow * src.h;

	dest.x = pos.x * dest.w;
	dest.y = pos.y * dest.h;

	Entity newEnt = { registry->create(), registry };
	newEnt.addComponent<TagComponent>(newEnt, "Tile");
	auto tile = &newEnt.addComponent<TileComponent>(newEnt, src, dest, *tileset.second.tilesetTex);

	if (tileColliders.count(tileID))
	{
		for (auto collider : tileColliders[tileID])
		{
			Entity newCollider = { registry->create(), registry };
			SDL_Rect colliderRect{ dest.x + collider.x, dest.y + collider.y, collider.w, collider.h };
			newCollider.addComponent<ColliderComponent>(newEnt, colliderRect);
		}
	}
	return tile;
}