#pragma once

#include "Event.h"

#include "SDL.h"

namespace Labyrinth {

	class LABYRINTH_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return mKeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keycode) 
			: mKeyCode(keycode) {}

		int mKeyCode;
	};	

	class LABYRINTH_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(Uint8 keycode, int repeatCount)
			: KeyEvent(keycode), mRepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return mRepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeat(s))";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int mRepeatCount;
	};

	class LABYRINTH_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(Uint8 keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}