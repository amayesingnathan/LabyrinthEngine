#include "Lpch.h"
#include "Application.h"

namespace Labyrinth {

	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		init("Labyrinth Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, configuration::SCREEN_WIDTH, configuration::SCREEN_HEIGHT, false);
		while (true)
		{

		}
	}

	void Application::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
	{
	}
}
