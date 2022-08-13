#pragma once

#include "Labyrinth/IO/KeyCodes.h"
#include "Labyrinth/IO/MouseButtonCodes.h"

#include <glm/glm.hpp>

namespace Labyrinth {

	class Input
	{
	public:
		static bool IsWindowHovered();

		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);

		static glm::vec2 GetMousePosition();
		static f32 GetMouseX();
		static f32 GetMouseY();

	};

}