#pragma once

#include "glm/glm.hpp"

#include "EventTypes.h"
#include "Labyrinth/IO/MouseCodes.h"

namespace Laby {

	struct MouseMovedEvent
	{
		f32 mouseX, mouseY;

		MouseMovedEvent(f32 x, f32 y) : mouseX(x), mouseY(y) {}

		EVENT_DATA_TYPE(MouseMoved)
	};

	struct MouseScrolledEvent
	{
		f32 xOffset, yOffset;

		MouseScrolledEvent(f32 x, f32 y) : xOffset(x), yOffset(y) {}

		EVENT_DATA_TYPE(MouseScrolled)
	};

	struct MouseButtonPressedEvent
	{
		MouseCode button;

		MouseButtonPressedEvent(MouseCode mouse) : button(mouse) {}

		EVENT_DATA_TYPE(MouseButtonPressed)
	};

	struct MouseButtonReleasedEvent
	{
		MouseCode button;

		MouseButtonReleasedEvent(MouseCode mouse) : button(mouse) {}

		EVENT_DATA_TYPE(MouseButtonReleased)
	};
}