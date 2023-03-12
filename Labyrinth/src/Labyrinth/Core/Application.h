#pragma once

#include <string>

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Core/LayerStack.h>
#include <Labyrinth/Core/Window.h>

#include <Labyrinth/Events/IEventListener.h>

#include <Labyrinth/ImGui/ImGuiHandler.h>

#include <Labyrinth/IO/Filesystem.h>

#include <Labyrinth/Networking/ServerLayer.h>
#include <Labyrinth/Networking/ClientLayer.h>

#include <Labyrinth/Scripting/ScriptEngine.h>

int main(int argc, char* argv[]);

namespace Laby {

	class Application;

	//To be defined in client
	extern Application* CreateApplication(int argc, char** argv);

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

		void onEvent(Event& e) override;

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThread();

	protected:
		template<IsLayer T, typename... Args>
		void PushLayer(Args&&... args) 
		{ 
			T* layer = new T(std::forward<Args>(args)...);
			PushLayer(layer);
		}
		void PushLayer(IsLayer auto* layer) 
		{ 
			mLayerStack.emplace_back(layer);  
			layer->onAttach();
		}

		template<IsClient T, typename... Args>
		void SetClient(Args&&... args)
		{
			mNetworkLayer = new T(std::forward<Args>(args)...);
			PushLayer(mNetworkLayer);
		}
		void SetClient()
		{
			mNetworkLayer = new ClientLayer();
			PushLayer(mNetworkLayer);
		}
		template<IsServer T, typename... Args>
		void SetServer(Args&&... args)
		{
			mNetworkLayer = new T(std::forward<Args>(args)...);
			PushLayer(mNetworkLayer);
		}

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
		static void BlockEvents(bool block);

	private:
		ApplicationSpec mSpecification;
		ApplicationState mState;
		Single<Window> mWindow;
		Single<ImGuiHandler> mImGuiHandler;
		std::vector<Layer*> mLayerStack;
		NetworkLayer* mNetworkLayer = nullptr;

		friend class SettingsModal;

	private:
		inline static Application* sInstance = nullptr;
		friend int ::main(int argc, char** argv);
	};
}