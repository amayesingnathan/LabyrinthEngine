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
		std::string spritePath = "assets/PlayerSprite.png";
		player.addComponent<SpriteComponent>(player, spritePath.c_str());
		auto& transform = player.getComponent<TransformComponent>();
		player.addComponent<KeyboardController>(player, &transform);
	}
}

