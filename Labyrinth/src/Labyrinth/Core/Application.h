#pragma once

#include "Labyrinth/Core/Base.h"

namespace Labyrinth { 

	class LABYRINTH_API Application
	{
	//Methods
	public:
		void run();

	private:
		void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	//Members
	public:

	private:
		bool mRunning = false;
		//std::unique_ptr<SDL_Window> window = nullptr;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

