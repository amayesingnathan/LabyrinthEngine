#include "Lpch.h"
#include "Labyrinth/IO/Input.h"

#include "Labyrinth/Common/Application.h"

#include "SDL.h"

#ifdef	LAB_PLATFORM_WINDOWS  //Input redefine for Windows!

namespace Labyrinth {

	bool Input::IsWindowFocused()
	{
		auto flags = SDL_GetWindowFlags(Cast<SDL_Window>(Application::Get().getWindow().getNativeWindow()));
		return flags & SDL_WINDOW_MOUSE_FOCUS;
	}

	bool Input::IsWindowHovered()
	{
		auto winSize = Application::Get().getWindow().getSize();
		auto pos = GetMousePosition();
		return ((pos.x > 0) && (pos.y > 0) && (pos.x < winSize.x) && (pos.y < winSize.y));
	}

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