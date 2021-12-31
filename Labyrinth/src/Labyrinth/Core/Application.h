	#pragma once

#include "Base.h"
#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>
#include "Window.h"

namespace Labyrinth { 
		
	class LABYRINTH_API Application
	{
	//Methods
	public:
		void run();

		void onEvent(Event& e);

	private:
		void init();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);

	//Members
	public:

	private:
		Single<Window> mWindow;
		bool mRunning = false;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

