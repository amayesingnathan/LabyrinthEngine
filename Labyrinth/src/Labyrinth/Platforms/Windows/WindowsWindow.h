#pragma once

#include "Lpch.h"

#include "Labyrinth/Core/Window.h"

#include "SDL.h"

namespace Labyrinth {


	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline unsigned int getWidth() const override { return mData.width; }
		inline unsigned int getHeight() const override { return mData.height; }

		inline void setEventCallback(const EventCallbackFunc& callback) override { mData.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

		void DispatchEvent();
		void DispatchWindowEvent();
		void DispatchKeyEvent();
		void DispatchMouseEvent();

	private:
		SDL_Window* mWindow;
		SDL_GLContext mContext;
		SDL_Event mEvent;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool vSync;

			EventCallbackFunc eventCallback;
		};

		WindowData mData;
	};

}

