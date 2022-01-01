#pragma once

#include "Lpch.h"

#include "Labyrinth/Events/Event.h"

namespace Labyrinth {

	struct WindowProps {
		std::string title;
		unsigned int width;
		unsigned int height;
		int flags;

		WindowProps(const std::string& t = "Labyrinth Engine",
			unsigned int w = 1280,
			unsigned int h = 720,
			//SDL not included here so use hex values that correspond to SDL_WINDOW_RESIZABLE and SDL_WINDOW_OPENGL
			int f = 0x00000020 | 0x00000002)
			: title(t), width(w), height(h), flags(f) {}
	};

	class Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		//Attributes
		virtual void setEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		static Single<Window> Create(const WindowProps& props = WindowProps());
	};

}