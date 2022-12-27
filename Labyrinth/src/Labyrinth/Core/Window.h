#pragma once

#include <functional>

#include "glm/glm.hpp"

#include "Labyrinth/Events/Event.h"
#include "Labyrinth/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Laby {

	struct WindowProps {
		std::string title;
		u32 width;
		u32 height;
		bool fullscreen;

		WindowProps(const std::string& t = "Labyrinth Engine",
			u32 w = 1600,
			u32 h = 900,
			bool f = false)
			: title(t), width(w), height(h), fullscreen(f) {}
	};

	class Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		Window(const WindowProps& props);
		virtual ~Window();

		void onUpdate();

		inline glm::vec2 getSize() const { return { mData.width, mData.height }; }
		inline u32 getWidth() const { return mData.width; }
		inline u32 getHeight() const { return mData.height; }

		void setTitle(const std::string& title);

		//Attributes
		void setEventCallback(const EventCallbackFunc& callback) { mData.eventCallback = callback; }
		void setVSync(bool enabled);
		bool isVSync() const;

		GLFWwindow* getNativeWindow() const { return mWindow; }
		GraphicsContext* getNativeContext() const { return mContext.get(); }

		static Single<Window> Create(const WindowProps& props = WindowProps());

	private:
		void Init(const WindowProps& props);
		void Shutdown();

	private:
		GLFWwindow* mWindow;
		Single<GraphicsContext> mContext;

		struct WindowData
		{
			std::string title;
			u32 width, height;
			bool vSync;

			EventCallbackFunc eventCallback;
		};

		WindowData mData;
	};

}