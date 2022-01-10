#pragma once

#include "Base.h"

#include "Window.h"
#include "LayerStack.h"

#include <Labyrinth/Core/Timestep.h>

#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>

#include <Labyrinth/ImGui/ImGuiLayer.h>

int main(int argc, char** argv);

namespace Labyrinth { 
		
	class Application
	{
	//Methods
	public:
		Application(const std::string& name = "Labyrinth Application");
		~Application();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		Window& getWindow() { return *mWindow; }

		static Application& Get() { return *sInstance; }

		void Close();

	private:
		void run();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	//Members
	public:

	private:
		static Application* sInstance;
		friend int ::main(int argc, char** argv);

		Single<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		bool mRunning = true;
		bool mMinimised = false;
		LayerStack mLayerStack;

		float mLastFrameTime = 0.0f;

	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

