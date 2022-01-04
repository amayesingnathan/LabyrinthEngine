#pragma once

#include "Event.h"

namespace Labyrinth {

	class KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return mKeyCode; }
		char getKeyName() const;

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keycode)
			: mKeyCode(keycode) {}

		int mKeyCode;
	};	

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), mRepeatCount(repeatCount) {}

		inline int getRepeatCount() const { return mRepeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeat(s))";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int mRepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(const std::string& text)
			: mText(text){}

		inline std::string getText() const { return mText; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
		EVENT_CLASS_TYPE(KeyTyped)

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << mText;
			return ss.str();
		}

	private:
		std::string mText;

	};
}