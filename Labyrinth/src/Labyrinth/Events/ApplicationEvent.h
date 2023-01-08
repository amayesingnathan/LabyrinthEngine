#pragma once

#include "EventTypes.h"

namespace Laby {

	struct WindowResizeEvent
	{
		u32 width, height;

		WindowResizeEvent(u32 w, u32 h) : width(w), height(h) {}

		EVENT_DATA_TYPE(WindowResize)
	};

	struct WindowCloseEvent
	{
		EVENT_DATA_TYPE(WindowClose)
	};

	struct WindowFocusEvent
	{
		EVENT_DATA_TYPE(WindowFocus)
	};

	struct WindowFocusLostEvent
	{
		EVENT_DATA_TYPE(WindowLostFocus)
	};

	struct WindowMovedEvent
	{
		u32 xpos, ypos;

		WindowMovedEvent(u32 x, u32 y) : xpos(x), ypos(y) {}

		EVENT_DATA_TYPE(WindowMoved)
	};


	struct AppTickEvent
	{
		EVENT_DATA_TYPE(AppTick)
	};

	struct AppUpdateEvent
	{
		EVENT_DATA_TYPE(AppUpdate)
	};

	struct AppRenderEvent
	{
		EVENT_DATA_TYPE(AppRender)
	};
}