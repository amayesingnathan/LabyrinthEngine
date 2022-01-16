#pragma once

#include <Labyrinth/Core/Window.h>
#include "Labyrinth/Renderer/GraphicsContext.h"

#include "SDL.h"

namespace Labyrinth {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline uint32_t getWidth() const override { return mData.width; }
		inline uint32_t getHeight() const override { return mData.height; }

		inline void setEventCallback(const EventCallbackFunc& callback) override { mData.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		inline virtual void* getNativeWindow() const { return mWindow; }
		inline virtual void* getNativeContext() const { return mContext.get(); }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

		void DispatchEvent();
		void DispatchWindowEvent();
		void DispatchKeyEvent();
		void DispatchTextEvent();
		void DispatchMouseEvent();

	private:
		SDL_Window* mWindow;
		Single<GraphicsContext> mContext;
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

