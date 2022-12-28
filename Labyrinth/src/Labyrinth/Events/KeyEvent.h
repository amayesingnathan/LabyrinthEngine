#pragma once

#include "EventTypes.h"
#include "Labyrinth/IO/KeyCodes.h"

namespace Laby {

	struct KeyPressedEvent
	{
		KeyCode keyCode;
		bool repeat;

		EVENT_DATA_TYPE(KeyPressed)
	};

	struct KeyReleasedEvent
	{
		KeyCode keyCode;

		EVENT_DATA_TYPE(KeyReleased)
	};

	struct KeyTypedEvent
	{
		KeyCode keyCode;

		EVENT_DATA_TYPE(KeyTyped)
	};
}