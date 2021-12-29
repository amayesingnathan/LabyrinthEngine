#pragma once

#include "Core.h"

namespace Labyrinth { 

	class LABYRINTH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

	private:
		void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

