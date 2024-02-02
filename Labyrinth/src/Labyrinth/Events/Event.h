#pragma once

#include "Events/IEventListener.h"

namespace Laby {

	using EventManager = slc::EventManager;
	using IEventListener = slc::IEventListener;

	using Event = slc::Event;
	using EventTypeFlag = slc::EventTypeFlag;
	namespace EventType = slc::EventType;

	using WindowCloseEvent = slc::WindowCloseEvent;
	using WindowResizeEvent = slc::WindowResizeEvent;
	using WindowFocusEvent = slc::WindowFocusEvent;
	using WindowFocusLostEvent = slc::WindowFocusLostEvent;
	using WindowMovedEvent = slc::WindowMovedEvent;

	using AppTickEvent = slc::AppTickEvent;
	using AppUpdateEvent = slc::AppUpdateEvent;
	using AppRenderEvent = slc::AppRenderEvent;

	using KeyPressedEvent = slc::KeyPressedEvent;
	using KeyReleasedEvent = slc::KeyReleasedEvent;
	using KeyTypedEvent = slc::KeyTypedEvent;

	using MouseButtonPressedEvent = slc::MouseButtonPressedEvent;
	using MouseButtonReleasedEvent = slc::MouseButtonReleasedEvent;

	using MouseMovedEvent = slc::MouseMovedEvent;
	using MouseScrolledEvent = slc::MouseScrolledEvent;

}
