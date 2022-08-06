#pragma once

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Core/Window.h>
#include <Labyrinth/Core/Timestep.h>
#include <Labyrinth/Core/LayerStack.h>

#include <Labyrinth/Events/ApplicationEvent.h>
#include <Labyrinth/Events/KeyEvent.h>
#include <Labyrinth/Events/MouseEvent.h>

#include <Labyrinth/ImGui/ImGuiLayer.h>

#include "Labyrinth/Scripting/ScriptEngine.h"

int main(int argc, char** argv);

namespace Labyrinth { 

	struct Resolution
	{
		u32 width, height;

		std::string toString() { return fmt::format("{}x{}", width, height); }
	};

	struct ApplicationSpec
	{
		std::string name = "Labyrinth Application";
		Resolution resolution = { 1600, 900 };
		std::filesystem::path workingDir;
		bool fullscreen = false;
		ScriptEngineConfig scriptConfig;
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

		ApplicationSpec& getSpec() { return mSpecification; }
		const ApplicationSpec& getSpec() const { return mSpecification; }

		static void ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec);
		static void WriteSettings(const std::filesystem::path& settingsPath);

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

		f32 mLastFrameTime = 0.0f;

	private:
		static Application* sInstance;
		friend int ::main(int argc, char** argv);

	};

	//To be defined in CLIENT
	Application* CreateApplication(int argc, char** argv);

}

