#pragma once

#include "EventTypes.h"

namespace Laby {

	struct WindowResizeEvent
	{
		u32 width, height;

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