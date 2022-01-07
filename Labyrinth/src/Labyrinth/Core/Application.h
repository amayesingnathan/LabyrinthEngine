	#pragma once

#include "Base.h"

#include "Window.h"
#include "LayerStack.h"
#include <Labyrinth/Events/Event.h>
#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>

#include <Labyrinth/ImGui/ImGuiLayer.h>

#include "Labyrinth/Renderer/Shader.h"
#include "Labyrinth/Renderer/VertexArray.h"

namespace Labyrinth { 
		
	class Application
	{
	//Methods
	public:
		Application();
		~Application() = default;

		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		Window& getWindow() { return *mWindow; }

		static Application& Get() { return *sInstance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);

	//Members
	public:

	private:
		static Application* sInstance;

		Single<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		bool mRunning = false;
		LayerStack mLayerStack;

		Ref<Shader> mShader;
		Ref<VertexArray> mVertexArray;

		Ref<Shader> mBlueShader;
		Ref<VertexArray> mSquareVA;
	};

	//To be defined in CLIENT
	Application* CreateApplication();

}

