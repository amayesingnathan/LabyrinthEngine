#pragma once

#include "Labyrinth/Core/System/Base.h"

namespace Laby {

	enum EventType : u32
	{
		None = 0,
		WindowClose = LAB_BIT(0), WindowResize = LAB_BIT(1), WindowFocus = LAB_BIT(2), WindowLostFocus = LAB_BIT(3), WindowMoved = LAB_BIT(4),
		AppTick = LAB_BIT(5), AppUpdate = LAB_BIT(6), AppRender = LAB_BIT(7),
		KeyPressed = LAB_BIT(8), KeyReleased = LAB_BIT(9), KeyTyped = LAB_BIT(10),
		MouseButtonPressed = LAB_BIT(11), MouseButtonReleased = LAB_BIT(12), MouseMoved = LAB_BIT(13), MouseScrolled = LAB_BIT(14)
	};

	using EventTypeFlag = u32;

	static constexpr EventTypeFlag EVENT_CATEGORY_APP	= WindowClose | WindowResize | WindowFocus | WindowLostFocus | WindowMoved | AppTick | AppUpdate | AppRender;
	static constexpr EventTypeFlag EVENT_CATEGORY_INPUT	= KeyPressed | KeyReleased | KeyTyped | MouseButtonPressed | MouseButtonReleased | MouseMoved | MouseScrolled;
	static constexpr EventTypeFlag EVENT_CATEGORY_KEY	= KeyPressed | KeyReleased | KeyTyped;
	static constexpr EventTypeFlag EVENT_CATEGORY_MOUSE	= MouseButtonPressed | MouseButtonReleased | MouseMoved | MouseScrolled;

#define EVENT_DATA_TYPE(type)	static EventType GetStaticType() { return EventType::type; }

}