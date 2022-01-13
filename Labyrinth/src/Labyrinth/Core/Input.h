#pragma once

#include "Labyrinth/Core/KeyCodes.h"
#include "Labyrinth/Core/MouseButtonCodes.h"

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