#pragma once

#include "SDL.h"

class Map
{
public: //Enums
	enum class Type {Grass = 0, Dirt = 1, Stone = 2};

public: //Methods

	Map();
	~Map();

	void init();

	void loadLevel(int lvl);
	void drawMap();

private: //Members
	SDL_Rect src, dest;
	SDL_Texture* textures;

	int map[40][50];

};

