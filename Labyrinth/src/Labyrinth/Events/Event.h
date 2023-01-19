#pragma once

#include <Labyrinth/Core/System/Reflection.h>

#include "ApplicationEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "NetworkEvent.h"

namespace Laby {

	using AllEvents = TypeList<
		WindowCloseEvent, WindowResizeEvent, WindowFocusEvent, WindowFocusLostEvent, WindowMovedEvent,
		AppTickEvent, AppUpdateEvent, AppRenderEvent,
		KeyPressedEvent, KeyReleasedEvent, KeyTypedEvent,
		MouseButtonPressedEvent, MouseButtonReleasedEvent,
		MouseMovedEvent, MouseScrolledEvent,
		NetMessageEvent
	>;

	template<typename T>
	concept IsEvent = AllEvents::Contains<T>;

	struct Event
	{
		bool handled = false;
		EventTypeFlag type = EventType::None;
		AllEvents::VariantType data;

		template<IsEvent TEvent, typename... TArgs>
		void init(TArgs&&... args) 
		{
			type = LAB_BIT(AllEvents::Index<TEvent>);
			data = TEvent(std::forward<TArgs>(args)...);
		}
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
