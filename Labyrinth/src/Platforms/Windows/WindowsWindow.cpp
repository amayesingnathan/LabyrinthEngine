#include "Lpch.h"
#include "WindowsWindow.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"
#include "Labyrinth/Events/KeyEvent.h"

#include "Labyrinth/Renderer/Renderer.h"

#include "OpenGL/OpenGLContext.h"

#include <Glad/glad.h>
#include <SDL_opengl.h>

namespace Labyrinth {

	static bool sSDLInitialised = false;

	Single<Window> Window::Create(const WindowProps& props)
	{
		return CreateSingle<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		LAB_PROFILE_FUNCTION();

		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		LAB_PROFILE_FUNCTION();

		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		LAB_PROFILE_FUNCTION();

		mData.title = props.title;
		mData.width = props.width;
		mData.height = props.height;

		LAB_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!sSDLInitialised)
		{
			LAB_PROFILE_SCOPE("SDLInit");

			int success = SDL_Init(SDL_INIT_EVERYTHING);
			LAB_CORE_ASSERT(!success, "Could not initialise SDL!");

			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			sSDLInitialised = true;	
		}

		SDL_GL_LoadLibrary(NULL);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		{
			LAB_PROFILE_SCOPE("glfwCreateWindow");

#if defined(LAB_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

			mWindow = SDL_CreateWindow(props.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, props.width, props.height, props.flags);
		}
		LAB_CORE_ASSERT(mWindow, "Could not create SDL window!");

		mContext = CreateSingle<OpenGLContext>(mWindow);
		mContext->init();

		SDL_SetWindowData(mWindow, "WindowData", &mData);
		setVSync(true);

	}

	void WindowsWindow::shutdown()
	{
		LAB_PROFILE_FUNCTION();

		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		LAB_CORE_TRACE("Shutdown complete");
	}


	void WindowsWindow::onUpdate()
	{
		LAB_PROFILE_FUNCTION();

		SDL_PollEvent(&mEvent);
		DispatchEvent();
		mContext->swapBuffers();
	}

	void WindowsWindow::setVSync(bool enabled)
	{
		LAB_PROFILE_FUNCTION();

		if (enabled)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

		mData.vSync = enabled;
	}

	bool WindowsWindow::isVSync() const
	{
		return mData.vSync;
	}

	void WindowsWindow::DispatchEvent()
	{
		/*
			Currently all events extract window data from the SDL_Window* in the WindowsWindow object,
			as opposed to using the SDL Window ID that the event happened in to find the SDL_Window* for the event.
			This is because if an event happens outside the main window (i.e. Dear ImGui viewport) it will have a 
			different Window ID, and fail to find the relevant window data for the event callback.
		*/
		switch (mEvent.type)
		{
		case SDL_WINDOWEVENT:
			DispatchWindowEvent();
			break;

		case SDL_KEYUP:
			DispatchKeyEvent();
			break;

		case SDL_KEYDOWN:
			DispatchKeyEvent();
			break;

		case SDL_TEXTINPUT:
			DispatchTextEvent();
			break;

		case SDL_MOUSEMOTION:
			DispatchMouseEvent();
			break;

		case SDL_MOUSEBUTTONDOWN:
			DispatchMouseEvent();
			break;

		case SDL_MOUSEBUTTONUP:
			DispatchMouseEvent();
			break;

		case SDL_MOUSEWHEEL:
			DispatchMouseEvent();
			break;
		}
	}

	void WindowsWindow::DispatchWindowEvent() {

		WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");
		if (mEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
			int w, h;
			SDL_GetWindowSize(mWindow, &w, &h);

			winData.width = w;
			winData.height = h;

			WindowResizeEvent event(mEvent.window.windowID, winData.width, winData.height);
			winData.eventCallback(event);
		}

		if (mEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
			WindowCloseEvent event(mEvent.window.windowID);
			winData.eventCallback(event);
		}

		if (mEvent.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			WindowFocusEvent event(mEvent.window.windowID);
			winData.eventCallback(event);
		}

		if (mEvent.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			WindowFocusLostEvent event(mEvent.window.windowID);
			winData.eventCallback(event);
		}
	}

	void WindowsWindow::DispatchKeyEvent()
	{
		WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

		int keycode = mEvent.key.keysym.scancode;
		switch (mEvent.type)
		{
		case SDL_KEYUP:
		{
			KeyReleasedEvent event(keycode);
			winData.eventCallback(event);
			break;
		}
		case SDL_KEYDOWN:
		{
			int repeat = mEvent.key.repeat;
			KeyPressedEvent event(keycode, repeat);
			winData.eventCallback(event);
			break;
		}
		}
	
	}

	void WindowsWindow::DispatchTextEvent()
	{
		WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

		KeyTypedEvent event(mEvent.text.text);
		winData.eventCallback(event);
	}

	void WindowsWindow::DispatchMouseEvent()
	{

		switch (mEvent.type)
		{
		case SDL_MOUSEMOTION:
		{
			WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

			float x = static_cast<float>(mEvent.motion.x);
			float y = static_cast<float>(mEvent.motion.y);

			MouseMovedEvent event(x, y);
			winData.eventCallback(event);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

			int button = mEvent.button.button;

			MouseButtonPressedEvent event(button);
			winData.eventCallback(event);
			break;	
		}
		case SDL_MOUSEBUTTONUP:
		{
			WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

			int button = mEvent.button.button;

			MouseButtonReleasedEvent event(button);
			winData.eventCallback(event);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			WindowData& winData = *(WindowData*)SDL_GetWindowData(mWindow, "WindowData");

			float x = static_cast<float>(mEvent.wheel.x);
			float y = static_cast<float>(mEvent.wheel.y);

			MouseScrolledEvent event(x, y);
			winData.eventCallback(event);
			break;
		}
		}
	}
}