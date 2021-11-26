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
		SpriteComponent playerSprite = TextureManager::LoadTexture("assets/PlayerSprite.png", renderer);		
		m_Registry.emplace<SpriteComponent>(player, playerSprite);

		GeometryComponent playerGeometry;
		playerGeometry.x = 0;
		playerGeometry.y = 0;
		playerGeometry.w = 64;
		playerGeometry.h = 64;
		m_Registry.emplace<GeometryComponent>(player, playerGeometry);

		VelocityComponent playerVelocity;
		m_Registry.emplace<VelocityComponent>(player, playerVelocity);

		PhysicsComponent playerPhysics(0, 0, eGravity::GravityOFF);
		m_Registry.emplace<PhysicsComponent>(player, playerPhysics);
	}
}

