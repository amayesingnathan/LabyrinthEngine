	#pragma once

#include "Base.h"

#include "Window.h"
#include "LayerStack.h"
#include <Labyrinth/Events/Event.h>
#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>

namespace Labyrinth { 
		
	class Application
	{
	//Methods
	public:
		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

	private:
		void init();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);

	//Members
	public:

	private:
		Single<Window> mWindow;
		bool mRunning = false;
		LayerStack mLayerStack;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

