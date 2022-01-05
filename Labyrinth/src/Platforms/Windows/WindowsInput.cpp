#include "Lpch.h"
#include "WindowsInput.h"

#include "Labyrinth/Core/Application.h"

#include "SDL.h"

namespace Labyrinth {

	Input* Input::sInstance = new WindowsInput();

	bool WindowsInput::isKeyPressedImpl(int keycode)
	{
		auto keys = SDL_GetKeyboardState(NULL);
		return keys[keycode];
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button)
	{
		auto buttons = SDL_GetMouseState(NULL, NULL);
		return button & buttons;
	}

	std::pair<float, float> WindowsInput::getMousePositionImpl()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		return std::pair<float, float>(static_cast<float>(x), static_cast<float>(y));
	}

	float WindowsInput::getMouseXImpl()
	{
		auto [x, y] = getMousePositionImpl();
		return x;
	}

	float WindowsInput::getMouseYImpl()
	{
		auto [x, y] = getMousePositionImpl();
		return y;
	}

}
