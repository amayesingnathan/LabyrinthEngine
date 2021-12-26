#include "ECS\Systems\InputManager.h"

#include "SDL.h"

#include "Labyrinth.h"
#include "Scene.h"

#include "ECS/Components/KeyboardController.h"
#include "ECS/Components/SpriteComponent.h"
#include "ECS/Components/VelocityComponent.h"

void InputManager::update()
{

	//Get entities that have Keyboard control and handle their key events
	auto players = mScene->mRegistry.view<KeyboardController, VelocityComponent>();

	for (auto control : players)
	{
		if (Labyrinth::keyboard[SDL_SCANCODE_ESCAPE])
		{
			Labyrinth::isRunning = false;
			return;
		}

		auto& keyControl = players.get<KeyboardController>(control);
		auto& velocity = players.get<VelocityComponent>(control);

		updateVelocity(velocity);
	}
}


void InputManager::updateVelocity(VelocityComponent& vel)
{
	vel.vel = 0.0f;

	//If up is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_W])
	{
		vel.vel.y -= moveSpeed;
	}

	//If down is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_S])
	{
		vel.vel.y += moveSpeed;
	}

	//If left is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_A])
	{
		vel.vel.x -= moveSpeed;
	}

	//If right is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_D])
	{
		vel.vel.x += moveSpeed;
	}

	vel.vel /= moveSpeed;
	vel.vel.normalise();
	vel.vel *= moveSpeed;
}
