#pragma once

#include "Labyrinth/Events/Event.h"
#include "Labyrinth/IO/MouseButtonCodes.h"

#include "glm/glm.hpp"

namespace Labyrinth {

	class LAB_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: mMouseX(x), mMouseY(y) {}

		glm::vec2 getPos() const { return { mMouseX, mMouseY }; }
		float getX() const { return mMouseX; }
		float getY() const { return mMouseY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float mMouseX, mMouseY;
	};

	class LAB_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: mXOffset(xOffset), mYOffset(yOffset) {}

		float getXOffset() const { return mXOffset; }
		float getYOffset() const { return mYOffset; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float mXOffset, mYOffset;
	};

	class LAB_API MouseButtonEvent : public Event
	{
	public:
		MouseCode getMouseButton() const { return mButton; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(const MouseCode button)
			: mButton(button) {}

		MouseCode mButton;
	};

	class LAB_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << mButton;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class LAB_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << mButton;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}