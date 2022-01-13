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
		return LAB_MOUSE_BUTTON(button) & buttons;
	}

	glm::vec2 Input::GetGlobalMousePosition()
	{
		int x, y;
		SDL_GetGlobalMouseState(&x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::GetGlobalMouseX()
	{
		return GetGlobalMousePosition().x;
	}

	float Input::GetGlobalMouseY()
	{
		return GetGlobalMousePosition().y;
	}

	glm::vec2 Input::GetMousePosition()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}

#endif