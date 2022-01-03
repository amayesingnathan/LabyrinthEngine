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
		Application();
		~Application() {};

		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline Window& getWindow() { return *mWindow; }

		static inline Application& Get() { return *sInstance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);

	//Members
	public:

	private:
		static Application* sInstance;

		Single<Window> mWindow;
		bool mRunning = false;
		LayerStack mLayerStack;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

