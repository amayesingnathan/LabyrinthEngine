#pragma once

#include "Lpch.h"

#include "Labyrinth/Events/Event.h"
#include "Labyrinth/Renderer/GraphicsContext.h"

#include "glm/glm.hpp"

struct GLFWwindow;

namespace Labyrinth {

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
		inline void setEventCallback(const EventCallbackFunc& callback) { mData.eventCallback = callback; }
		void setVSync(bool enabled);
		bool isVSync() const;

		inline GLFWwindow* getNativeWindow() const { return mWindow; }
		inline GraphicsContext* getNativeContext() const { return mContext.get(); }

		static Single<Window> Create(const WindowProps& props = WindowProps());

	private:
		void init(const WindowProps& props);
		void shutdown();

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