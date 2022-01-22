#pragma once

#include "Event.h"

namespace Labyrinth {

	class WindowEvent : public Event
	{
	public:
		uint32_t getID() const { return mWindowID; }

	protected:
		WindowEvent(uint32_t id)
			: mWindowID(id) {}

	private:
		uint32_t mWindowID;
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(uint32_t id, uint32_t width, uint32_t height)
			: WindowEvent(id), mWidth(width), mHeight(height) {}

		uint32_t getWidth() const { return mWidth; }
		uint32_t getHeight() const { return mHeight; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		uint32_t mWidth, mHeight;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent(uint32_t id) 
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		WindowFocusEvent(uint32_t id)
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(WindowFocus)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowFocusLostEvent : public WindowEvent
	{
	public:
		WindowFocusLostEvent(uint32_t id)
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(WindowLostFoucus)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}