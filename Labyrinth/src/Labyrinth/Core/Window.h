#pragma once

#include "Lpch.h"

#include "Labyrinth/Events/Event.h"

#include "glm/glm.hpp"

namespace Labyrinth {

	struct LAB_API WindowProps {
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& t = "Labyrinth Engine",
			uint32_t w = 1600,
			uint32_t h = 900)
			: title(t), width(w), height(h) {}
	};

	class LAB_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual glm::vec2 getSize() const = 0;
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		//Attributes
		virtual void setEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;
		virtual void* getNativeContext() const = 0;

		static Single<Window> Create(const WindowProps& props = WindowProps());
	};

}	