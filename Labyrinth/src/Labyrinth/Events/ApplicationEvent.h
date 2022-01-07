#pragma once

#include "Event.h"

namespace Labyrinth {

	class WindowEvent : public Event
	{
	public:
		unsigned int getID() const { return mWindowID; }

	protected:
		WindowEvent(unsigned int id)
			: mWindowID(id) {}

	private:
		unsigned int mWindowID;
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(unsigned int id, unsigned int width, unsigned int height)
			: WindowEvent(id), mWidth(width), mHeight(height) {}

		unsigned int getWidth() const { return mWidth; }
		unsigned int getHeight() const { return mHeight; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int mWidth, mHeight;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent(unsigned int id) 
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	};

	class AppTickEvent : public WindowEvent
	{
	public:
		AppTickEvent(unsigned int id)
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public WindowEvent
	{
	public:
		AppUpdateEvent(unsigned int id)
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public WindowEvent
	{
	public:
		AppRenderEvent(unsigned int id)
			: WindowEvent(id) {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}