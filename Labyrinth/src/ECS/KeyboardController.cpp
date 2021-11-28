#include "ECS/KeyboardController.h"
#include "ECS/TransformComponent.h"
#include "ECS/Entity.h"
#include "Labyrinth.h"

KeyboardController::KeyboardController() : Component(), transform(nullptr) {}

KeyboardController::KeyboardController(Entity entt, TransformComponent* transform) :
	Component(entt), transform(transform) {}

void KeyboardController::update()
{
	if (Labyrinth::event.type == SDL_KEYDOWN)
	{
		switch (Labyrinth::event.key.keysym.sym)
		{
		case SDLK_w:
			transform->velocity->vel.y = -3;
			break;

		case SDLK_a:
			transform->velocity->vel.x = -3;
			break;

		case SDLK_s:
			transform->velocity->vel.y = 3;
			break;

		case SDLK_d:
			transform->velocity->vel.x = 3;
			break;

		case SDLK_ESCAPE:
			Labyrinth::isRunning = false;
			break;

		default:
			break;
		}
	}

	if (Labyrinth::event.type == SDL_KEYUP)
	{
		switch (Labyrinth::event.key.keysym.sym)
		{
		case SDLK_w:
			transform->velocity->vel.y = 0;
			break;

		case SDLK_a:
			transform->velocity->vel.x = 0;
			break;

		case SDLK_s:
			transform->velocity->vel.y = 0;
			break;

		case SDLK_d:
			transform->velocity->vel.x = 0;
			break;


		default:
			break;
		}
	}
}