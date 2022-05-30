#pragma once

#include "Lpch.h"

#include "Labyrinth/Events/Event.h"
#include "Labyrinth/Renderer/GraphicsContext.h"

#include "glm/glm.hpp"

struct GLFWwindow;

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

		Window(const WindowProps& props);
		virtual ~Window();

		void onUpdate();

		inline glm::vec2 getSize() const { return { mData.width, mData.height }; }
		inline uint32_t getWidth() const { return mData.width; }
		inline uint32_t getHeight() const { return mData.height; }

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
			unsigned int width, height;
			bool vSync;

			EventCallbackFunc eventCallback;
		};

		WindowData mData;
	};

}	