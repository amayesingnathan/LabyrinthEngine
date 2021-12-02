#include "Game.h"
#include "TextureManager.h"

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
		std::string playerSpritePath = "assets/PlayerSprite.png";
		player.addComponent<SpriteComponent>(player, playerSpritePath.c_str(), 4, 2, 8, 14);
		player.addComponent<KeyboardController>(player);
		player.addComponent<ColliderComponent>(player);

		std::string wallSpritePath = "assets/textures/worldtextures.png";
		auto& sprite = testWall.addComponent<SpriteComponent>(testWall, wallSpritePath.c_str(), 32, 288, 32, 32);
		testWall.addComponent<ColliderComponent>(testWall);
	}
}

