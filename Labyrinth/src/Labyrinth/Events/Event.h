#pragma once

#include <variant>

#include "ApplicationEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"

namespace Laby {

	using EventData = std::variant<
		WindowResizeEvent, WindowCloseEvent, WindowFocusEvent, WindowFocusLostEvent,
		AppTickEvent, AppUpdateEvent, AppRenderEvent,
		KeyPressedEvent, KeyReleasedEvent, KeyTypedEvent,
		MouseMovedEvent, MouseScrolledEvent,
		MouseButtonPressedEvent, MouseButtonReleasedEvent
	>;

	struct Event
	{
		bool handled = false;
		EventType type = EventType::None;
		EventData data;
	};

	class LocalEventDispatcher
	{
	public:
		LocalEventDispatcher(Event& event)
			: mEvent(event) {}

		template<typename T>
		using EventFunc = std::function<bool(T&)>;

		template<typename T>
		void dispatch(EventFunc<T> func)
		{
			if (mEvent.type != T::GetStaticType())
				return;

			if (mEvent.handled)
				return;

			mEvent.handled = func(std::get<T>(mEvent.data));
		}

	private:
		Event& mEvent;
	};
}
