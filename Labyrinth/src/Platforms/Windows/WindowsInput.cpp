#include "Lpch.h"
#include "Labyrinth/Core/Input.h"

#include "Labyrinth/Core/Application.h"

#include "SDL.h"

#ifdef	LAB_PLATFORM_WINDOWS  //Input redefine for Windows!

namespace Labyrinth {

	bool Input::IsKeyPressed(int keycode)
	{
		auto keys = SDL_GetKeyboardState(NULL);
		return keys[keycode];
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto buttons = SDL_GetMouseState(NULL, NULL);
		return button & buttons;
	}

	glm::vec2 Input::GetMousePosition()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::GetMouseX()
	{
		auto pos = GetMousePosition();
		return pos.x;
	}

	float Input::GetMouseY()
	{
		auto pos = GetMousePosition();
		return pos.y;
	}

}

#endif