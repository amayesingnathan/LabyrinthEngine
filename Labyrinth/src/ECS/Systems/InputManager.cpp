#include "ECS\Systems\InputManager.h"

#include "SDL.h"

#include "Labyrinth.h"

#include "ECS/Components/KeyboardController.h"
#include "ECS/Components/SpriteComponent.h"

void InputManager::update()
{

	//Get entities that have Keyboard control and handle their key events
	auto players = registry->view<KeyboardController, VelocityComponent, SpriteComponent>();

	for (auto control : players)
	{
		auto& keyControl = players.get<KeyboardController>(control);
		auto& velocity = players.get<VelocityComponent>(control);
		auto& sprite = players.get<SpriteComponent>(control);

		//Reset velocity but save prev velocity
		Vector2D prevVel = velocity.vel;

		updateVelocity(velocity);

		sprite.spriteFlip = (velocity.vel.x >= 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		if (prevVel.x != velocity.vel.x)
		{
			if (keyControl.lerpTime.x < Vector2D::lerpDur)
			{
				velocity.vel.x = Vector2D::lerp(prevVel.x, velocity.vel.x, keyControl.lerpTime.x);
				keyControl.lerpTime.x += configuration::frameDelay;
			}
			else { keyControl.lerpTime.x = 0.0f; }
		}
		if (prevVel.y != velocity.vel.y)
		{
			if (keyControl.lerpTime.y < Vector2D::lerpDur)
			{
				velocity.vel.y = Vector2D::lerp(prevVel.y, velocity.vel.y, keyControl.lerpTime.y);
				keyControl.lerpTime.y += configuration::frameDelay;
			}
			else { keyControl.lerpTime.y = 0.0f; }
		}
	}
}

/*

	NEEDS FIXING!! RESETS CHANGES FROM ACCELERATION IN PHYSICS SYSTEM - NEEDS TO ADD TO THEM
	BUT THEN HAVE MAX SPEED (MAYBE ADD TO VELOCITY COMPONENT)

*/

void InputManager::updateVelocity(VelocityComponent& vel)
{
	vel.vel = 0.0f;
	//If up is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_W])
	{
		vel.vel.y -= 1.0f;
	}

	//If down is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_S])
	{
		vel.vel.y += 1.0f;
	}

	//If left is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_A])
	{
		vel.vel.x -= 1.0f;
	}

	//If right is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_D])
	{
		vel.vel.x += 1.0f;
	}

	//Normalise the vector so can't travel faster diagonally
	vel.vel.normalise();

	//Scale to player speed
	vel.vel *= moveSpeed;
}