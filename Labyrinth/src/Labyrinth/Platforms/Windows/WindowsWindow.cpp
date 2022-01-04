#include "Lpch.h"
#include "WindowsWindow.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"
#include "Labyrinth/Events/KeyEvent.h"

#include "imgui.h"

#include <Glad/glad.h>
#include <SDL_opengl.h>

namespace Labyrinth {

	static bool sSDLInitialised = false;
	static bool sGladInitialised = false;

	Single<Window> Window::Create(const WindowProps& props)
	{
		return CreateSingle<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		mData.title = props.title;
		mData.width = props.width;
		mData.height = props.height;

		LAB_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!sSDLInitialised)
		{
			int success = SDL_Init(SDL_INIT_EVERYTHING);
			LAB_CORE_ASSERT(!success, "Could not initialise SDL!");

			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			sSDLInitialised = true;	
		}

		SDL_GL_LoadLibrary(NULL);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		mWindow = SDL_CreateWindow(props.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, props.width, props.height, props.flags);
		LAB_CORE_ASSERT(mWindow, "Could not create SDL window!")

		mContext = SDL_GL_CreateContext(mWindow);
		LAB_CORE_ASSERT(mContext, "Could not create OpenGL context!");

		SDL_GL_MakeCurrent(mWindow, mContext);

		if (!sGladInitialised)
		{
			int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
			LAB_CORE_ASSERT(status, "Could not initialise Glad!");

			sGladInitialised = true;
		}

		SDL_SetWindowData(mWindow, "WindowData", &mData);
		setVSync(true);

	}

	void WindowsWindow::shutdown()
	{
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		LAB_CORE_TRACE("Shutdown complete");
	}


	void WindowsWindow::onUpdate()
	{
		SDL_PollEvent(&mEvent);
		DispatchEvent();
		SDL_GL_SwapWindow(mWindow);
	}

	void WindowsWindow::setVSync(bool enabled)
	{
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

		SDL_Window* win = SDL_GetWindowFromID(mEvent.window.windowID);
		WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");
		if (mEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
			int w, h;
			SDL_GetWindowSize(win, &w, &h);

			winData.width = w;
			winData.height = h;

			WindowResizeEvent event(winData.width, winData.height);
			winData.eventCallback(event);
		}

		if (mEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
			WindowCloseEvent event;
			winData.eventCallback(event);
		}
		
	}

	void WindowsWindow::DispatchKeyEvent()
	{
		SDL_Window* win = SDL_GetWindowFromID(mEvent.key.windowID);
		WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

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
		SDL_Window* win = SDL_GetWindowFromID(mEvent.text.windowID);
		WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

		KeyTypedEvent event(mEvent.text.text);
		winData.eventCallback(event);
	}

	void WindowsWindow::DispatchMouseEvent()
	{

		switch (mEvent.type)
		{
		case SDL_MOUSEMOTION:
		{
			SDL_Window* win = SDL_GetWindowFromID(mEvent.motion.windowID);
			WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

			int x = mEvent.motion.x;
			int y = mEvent.motion.y;

			MouseMovedEvent event(x, y);
			winData.eventCallback(event);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			SDL_Window* win = SDL_GetWindowFromID(mEvent.button.windowID);
			WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

			int button = mEvent.button.button;

			MouseButtonPressedEvent event(button);
			winData.eventCallback(event);
			break;	
		}
		case SDL_MOUSEBUTTONUP:
		{
			SDL_Window* win = SDL_GetWindowFromID(mEvent.button.windowID);
			WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

			int button = mEvent.button.button;

			MouseButtonPressedEvent event(button);
			winData.eventCallback(event);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			SDL_Window* win = SDL_GetWindowFromID(mEvent.wheel.windowID);
			WindowData& winData = *(WindowData*)SDL_GetWindowData(win, "WindowData");

			int x = mEvent.wheel.x;
			int y = mEvent.wheel.y;

			MouseScrolledEvent event(x, y);
			winData.eventCallback(event);
			break;
		}
		}
	}
}