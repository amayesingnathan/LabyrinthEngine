#include "ECS/KeyboardController.h"
#include "ECS/VelocityComponent.h"
#include "ECS/Entity.h"
#include "Labyrinth.h"

float Vector2D::lerpDur = 0.1f;

KeyboardController::KeyboardController() : Component(), velocity(nullptr), lerpTime(0.0f) {}

KeyboardController::KeyboardController(Entity entt) :
	Component(entt), lerpTime(0.0f)
{
	velocity = &entity.getComponent<VelocityComponent>();
}

void KeyboardController::update()
{
	//Reset velocity but save prev velocity
	Vector2D prevVel = velocity->vel;

	updateVelocity(*velocity);

	if (prevVel.x != velocity->vel.x)
	{
		if (lerpTime.x < Vector2D::lerpDur)
		{ 
			velocity->vel.x = Vector2D::lerp(prevVel.x, velocity->vel.x, lerpTime.x);
			//if (velocity->vel.x > prevVel.x) { velocity->vel.x = Vector2D::lerp(prevVel.x, velocity->vel.x, lerpTime.x / Vector2D::lerpDur); }
			//else { velocity->vel.x = Vector2D::lerp(velocity->vel.x, prevVel.x, 1 - (lerpTime.x / Vector2D::lerpDur)); }
			lerpTime.x += configuration::frameDelay;
		}
		else { lerpTime.x = 0.0f; }
	}
	if (prevVel.y != velocity->vel.y)
	{
		if (lerpTime.y < Vector2D::lerpDur)
		{
			velocity->vel.y = Vector2D::lerp(prevVel.y, velocity->vel.y, lerpTime.y);
			//if (velocity->vel.y > prevVel.y) { velocity->vel.y = Vector2D::lerp(prevVel.y, velocity->vel.y, lerpTime.y / Vector2D::lerpDur); }
			//else { velocity->vel.y = Vector2D::lerp(velocity->vel.y, prevVel.y, 1 - (lerpTime.y / Vector2D::lerpDur)); }
			lerpTime.y += configuration::frameDelay;
		}
		else { lerpTime.y = 0.0f; }
	}
}


void KeyboardController::updateVelocity(VelocityComponent& vel)
{
	vel.vel = 0.0f;
    //If up is pressed
    if (Labyrinth::keyboard[SDL_SCANCODE_W])
    {
		vel.vel.y -= 3.0f;
    }

    //If down is pressed
    if (Labyrinth::keyboard[SDL_SCANCODE_S])
    {
		vel.vel.y += 3.0f;
	}

    //If left is pressed
    if (Labyrinth::keyboard[SDL_SCANCODE_A])
    {
		vel.vel.x -= 3.0f;
	}

    //If right is pressed
	if (Labyrinth::keyboard[SDL_SCANCODE_D])
	{
		vel.vel.x += 3.0f;
	}
}