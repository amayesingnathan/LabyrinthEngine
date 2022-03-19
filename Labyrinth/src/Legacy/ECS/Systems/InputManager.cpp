#include "Lpch.h"

#if 0

#include "./InputManager.h"

#include "Labyrinth.h"
#include "Scene.h"

#include "ECS/Components/GameComponents.h"

void InputManager::update()
{

	//Get entities that have Keyboard control and handle their key events
	auto players = mScene->mRegistry.view<KeyboardController, VelocityComponent>();

	for (auto entity : players)
	{
		if (Labyrinth::keyboard[SDL_SCANCODE_ESCAPE])
		{
			Labyrinth::isRunning = false;
			return;
		}

		updateVelocity(entity);
	}
}


void InputManager::updateVelocity(entt::entity entity)
{
	auto& vel = mScene->mRegistry.get<VelocityComponent>(entity);
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

#endif