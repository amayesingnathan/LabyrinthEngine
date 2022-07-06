#pragma once

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Core/Window.h>
#include <Labyrinth/Core/Timestep.h>
#include <Labyrinth/Core/LayerStack.h>


#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>

#include <Labyrinth/ImGui/ImGuiLayer.h>

int main(int argc, char** argv);

namespace Labyrinth { 

	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const
		{
			LAB_CORE_ASSERT(index < count);
			return args[index];
		}
	};

	struct ApplicationSpec
	{
		std::string name = "Labyrinth Application";
		ApplicationCommandLineArgs commandLineArgs;
		std::string workingDir;
	};

	class Application
	{
	//Methods
	public:
		Application(const ApplicationSpec& spec);
		~Application();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		Window& getWindow() { return *mWindow; }

		static Application& Get() { return *sInstance; }

		static void BlockEsc(bool block = true) { sInstance->mBlockExit = block; }

		void Close();

		ImGuiLayer* getImGuiLayer() { return mImGuiLayer; }

		const ApplicationSpec& getSpec() const { return mSpecification; }

	private:
		void run();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

	//Members
	public:

	private:
		ApplicationSpec mSpecification;
		Single<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		bool mRunning = true;
		bool mMinimised = false;
		bool mBlockExit = false;
		LayerStack mLayerStack;

		float mLastFrameTime = 0.0f;

	private:
		static Application* sInstance;
		friend int ::main(int argc, char** argv);

	};

	//To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);

}

