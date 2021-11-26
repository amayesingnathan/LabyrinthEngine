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
		SpriteComponent playerSprite;
		playerSprite.texture = TextureManager::LoadTexture("assets/PlayerSprite.png");
		m_Registry.emplace<SpriteComponent>(player, playerSprite);

		TransformComponent playerTransform;
		playerTransform.position.x = 0;
		playerTransform.position.y = 0;
		m_Registry.emplace<TransformComponent>(player, playerTransform);

		VelocityComponent playerVelocity;
		m_Registry.emplace<VelocityComponent>(player, playerVelocity);

		PhysicsComponent playerPhysics;
		m_Registry.emplace<PhysicsComponent>(player, playerPhysics);
	}
}

