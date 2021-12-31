#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Labyrinth/Platforms/Windows/WindowsWindow.h"

#include "SDL.h"
#include "GL/glew.h"
#include "SDL_opengl.h"

namespace Labyrinth {

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

	void Application::run()
	{
		init();
		mRunning = true;

		while (mRunning)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			mWindow->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		LAB_CORE_TRACE("{0}", e);
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(Application::OnKeyPress));
	}

	void Application::init()
	{
		mWindow = Window::Create();
		mWindow->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnKeyPress(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case SDL_SCANCODE_ESCAPE:
			mRunning = false;
			break;

		default:
			break;
		}
		return true;
	}
}
