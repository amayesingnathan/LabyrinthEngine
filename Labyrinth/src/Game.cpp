#include "Game.h"

Game::Game() :
	cnt(0)
{
}

Game::~Game()
{
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	Labyrinth::init(title, xpos, ypos, width, height, fullscreen);
	if (running())
	{
		player.addComponent<PlayerComponent>(player, SDL_Rect{ 0, 0, 10, 14 }, 3);

		std::string wallSpritePath = "assets/textures/worldtextures.png";
		testWall.addComponent<SpriteComponent>(testWall, wallSpritePath.c_str(), SDL_Rect{32, 288, 32, 32});
		testWall.addComponent<ColliderComponent>(testWall);
	}
}

 