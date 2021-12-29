#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Labyrinth/Events/ApplicationEvent.h"

namespace Labyrinth {

	void Application::run()
	{
		WindowResizeEvent e(1200, 800);
		LAB_TRACE(e);

		while (mRunning)
		{

		}
	}

	void Application::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
	{

	}
}
