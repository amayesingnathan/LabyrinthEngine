#pragma once
#include "Labyrinth.h"

class Game : public Labyrinth
{
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

private:
	int cnt;
};

