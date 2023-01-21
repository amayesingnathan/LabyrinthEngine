#pragma once

#include <string>

#include "Labyrinth/Core/System/Base.h"
#include <Labyrinth/Core/LayerStack.h>
#include <Labyrinth/Core/Window.h>

#include <Labyrinth/Events/IEventListener.h>

#include <Labyrinth/ImGui/ImGuiLayer.h>

#include <Labyrinth/IO/Filesystem.h>

#include <Labyrinth/Networking/ServerLayer.h>
#include <Labyrinth/Networking/ClientLayer.h>

#include <Labyrinth/Scripting/ScriptEngine.h>

int main(int argc, char* argv[]);

namespace Laby {

	struct Resolution
	{
		u32 width, height;

		std::string toString() const { return fmt::format("{}x{}", width, height); }
	};

	struct ApplicationSpec
	{
		std::string name = "Labyrinth Application";
		Resolution resolution = { 1600, 900 };
		fs::path workingDir;
		bool fullscreen = false;
		fs::path startupProject;
		ScriptEngineConfig scriptConfig;
	};

	struct ApplicationState
	{
		bool running = true;
		bool minimised = false;
		bool blockExit = false;
		f32 lastFrameTime = 0.0f;

		std::vector<Action<>> mainThreadQueue;
		std::mutex mainThreadQueueMutex;
	};

	class Application : public IEventListener
	{
	public:
		LISTENING_EVENTS(WindowClose, WindowResize)

	public:
		Application(const ApplicationSpec& spec);
		~Application();

	public:
		Window& getWindow() { return *mWindow; }
		ImGuiLayer* getImGuiLayer() { return mImGuiLayer; }

		void onEvent(Event& e) override;

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		void ExecuteMainThread();

	protected:
		void PushLayer(Layer* layer) { mLayerStack.pushLayer(layer); }
		void PushOverlay(Layer* overlay) { mLayerStack.pushOverlay(overlay); }

		void SetClient(ClientLayer* client = nullptr);
		void SetServer(ServerLayer* server);

	public:
		static void Run(int argc, char** argv);
		static Application& Get() { return *sInstance; }
		static void Close();

		static const ApplicationSpec& GetSpec() { return sInstance->mSpecification; }

		static void SubmitActionToMainThread(const Action<>& function);
		static void SendNetMessage(const Message& msg);

		static void ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec);
		static void WriteSettings(const std::filesystem::path& settingsPath);

		static void BlockEsc(bool block = true);

	private:
		ApplicationSpec mSpecification;
		ApplicationState mState;
		Single<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		NetworkLayer* mNetworkLayer = nullptr;
		LayerStack mLayerStack;

		friend class SettingsModal;

	private:
		inline static Application* sInstance = nullptr;
		friend int ::main(int argc, char** argv);
	};

	//To be defined in client
	extern Application* CreateApplication(int argc, char** argv);
}