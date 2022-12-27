#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <sstream>

namespace Laby {

	enum class EventType
	{
		None = -1,
		WindowClose, WindowResize, WindowFocus, WindowLostFoucus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None					 = 0,
		EventCategoryApplication = LAB_BIT(0),
		EventCategoryInput		 = LAB_BIT(1),
		EventCategoryKeyboard	 = LAB_BIT(2),
		EventCategoryMouse		 = LAB_BIT(3),
		EventCategoryMouseButton = LAB_BIT(4)
	};

#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return GetStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual i32 getCategoryFlags() const override { return category; }

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual ~Event() = default;

		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual i32 getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
		}

	public:
		bool handled = false;

	private:
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: mEvent(event) {}

		//F deduced by compiler
		template<typename T, typename F>
		void dispatch(const F& func)
		{
			if (mEvent.getEventType() != T::GetStaticType())
				return;

			if (mEvent.handled)
				return;

			mEvent.handled = func(static_cast<T&>(mEvent));
		}

	private:
		Event& mEvent;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}

}
