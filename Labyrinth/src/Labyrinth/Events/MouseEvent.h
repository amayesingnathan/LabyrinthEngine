#pragma once

#include "glm/glm.hpp"

#include "EventTypes.h"
#include "Labyrinth/IO/MouseCodes.h"

namespace Laby {

	struct MouseMovedEvent
	{
		f32 mouseX, mouseY;

		EVENT_DATA_TYPE(MouseMoved)
	};

	struct MouseScrolledEvent
	{
		f32 xOffset, yOffset;

		EVENT_DATA_TYPE(MouseScrolled)
	};

	struct MouseButtonPressedEvent
	{
		MouseCode button;

		EVENT_DATA_TYPE(MouseButtonPressed)
	};

	struct MouseButtonReleasedEvent
	{
		MouseCode button;

		EVENT_DATA_TYPE(MouseButtonReleased)
	};
}