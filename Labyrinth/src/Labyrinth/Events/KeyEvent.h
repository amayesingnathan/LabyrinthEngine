#pragma once

#include "Labyrinth/Events/Event.h"
#include "Labyrinth/IO/KeyCodes.h"

namespace Labyrinth {

	class KeyEvent : public Event
	{
	public:
		KeyCode getKeyCode() const { return mKeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(const KeyCode keycode)
			: mKeyCode(keycode) {}

		KeyCode mKeyCode;
	};	

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool repeat = false)
			: KeyEvent(keycode), mIsRepeat(repeat) {}

		bool isRepeated() const { return mIsRepeat; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mIsRepeat << " repeat(s))";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool mIsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
		EVENT_CLASS_TYPE(KeyTyped)

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << mKeyCode;
			return ss.str();
		}

	};
}