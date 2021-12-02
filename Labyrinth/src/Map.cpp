#include "Map.h"

#include "TextureManager.h"
#include <fstream>
#include <iostream>
#include <string>

Map::Map() :
	src(), dest()
{
	for (int row = 0; row < 40; row++)
	{
		for (int col = 0; col < 50; col++)
		{
			map[row][col] = -1;
		}
	}

	textures = NULL;

	src.w = src.h = 32;

	dest.w = dest.h = 16;

}

Map::~Map()
{
	TextureManager::DestroyTexture(textures);
}

void Map::init()
{
	textures = TextureManager::LoadTexture("assets/textures/worldtextures.png");
}

void Map::loadLevel(int lvl)
{
	std::string lvlPath = "levels/level";
	std::string fileType = ".lvl";

	lvlPath = lvlPath + std::to_string(lvl) + fileType;

	std::ifstream tiles(lvlPath);
	if (tiles)
	{
		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 50; j++)
			{
				tiles >> map[i][j];
			}
		}
	}
	else
	{
		std::cout << "Could not open level file." << std::endl;
	}
}

void Map::drawMap()
{
	int fileType;

	for (int row = 0; row < 40; row++)
	{
		for (int col = 0; col < 50; col++)
		{
			fileType = map[row][col];
			dest.x = col * 16;
			dest.y = row * 16;
			Type type = static_cast<Type>(fileType);

			switch (type)
			{
			case Type::Grass:
				//Grass texture location in sheet
				src = TextureManager::setSubTex(320, 640);
				TextureManager::Draw(textures, src, dest);
				break;

			case Type::Dirt:
				//Dirt texture location in sheet
				src = TextureManager::setSubTex(0, 576);
				TextureManager::Draw(textures, src, dest);
				break;

			case Type::Stone:
				//Stone texture location in sheet
				src = TextureManager::setSubTex(96, 896);
				TextureManager::Draw(textures, src, dest);
				break;

			case Type::Invalid:
				break;

			default:
				break;
			}	
		}
	}
}
