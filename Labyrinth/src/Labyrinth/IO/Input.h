#pragma once

#include "Labyrinth/IO/KeyCodes.h"
#include "Labyrinth/IO/MouseButtonCodes.h"

#include <glm/glm.hpp>

namespace Labyrinth {

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button);

		static glm::vec2 GetGlobalMousePosition();
		static float GetGlobalMouseX();
		static float GetGlobalMouseY();

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	};

}