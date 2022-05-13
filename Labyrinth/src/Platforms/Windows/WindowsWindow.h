#pragma once

#include <Labyrinth/Core/Window.h>
#include "Labyrinth/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Labyrinth {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
			
		void onUpdate() override;

		inline glm::vec2 getSize() const override { return { mData.width, mData.height }; }
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
		GLFWwindow* mWindow;
		Single<GraphicsContext> mContext;

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

