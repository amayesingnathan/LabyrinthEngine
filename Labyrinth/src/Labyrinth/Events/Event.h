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

		template<IsEvent T>
		void dispatch(Predicate<T&>&& func)
		{
			if (type != T::GetStaticType())
				return;

			if (handled)
				return;

			handled = func(std::get<T>(data));
		}
	};
}
