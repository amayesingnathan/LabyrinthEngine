#include "Lpch.h"
#include "Application.h"

#include "ImGuizmo.h"

#include <Labyrinth/IO/JSON.h>
#include <Labyrinth/Networking/ServerLayer.h>
#include <Labyrinth/Networking/ClientLayer.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Tools/Timing.h>

namespace Laby {

	Application::Application(const ApplicationSpec& spec)
		: IEventListener(ListenerType::App), mSpecification(spec)
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		if (!mSpecification.workingDir.empty())
			std::filesystem::current_path(mSpecification.workingDir);

		mWindow = Window::Create(WindowProps(mSpecification.name, mSpecification.resolution.width, mSpecification.resolution.height, mSpecification.fullscreen));

		Renderer::Init();
		ScriptEngine::Init(spec.scriptConfig);

		mImGuiHandler = MakeSingle<ImGuiHandler>();
	}

	Application::~Application()
	{
		for (Layer* layer : mLayerStack)
		{
			layer->onDetach();
			delete layer;
		}
		mImGuiHandler.reset();

		ScriptEngine::Shutdown();
		Renderer::Shutdown();
		Project::SetInactive();
	}

	void Application::onEvent(Event& e)
	{
		LocalEventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(LAB_BIND_EVENT_FUNC(OnWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(OnWindowResize));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Application::Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{

		if (e.width == 0 || e.height == 0)
		{
			mState.minimised = true;
			return false;
		}

		mState.minimised = false;
		//Renderer::OnWindowResize(e.getWidth(), e.getHeight());
		return false;
	}

	void Application::ExecuteMainThread()
	{
		std::scoped_lock<std::mutex> lock(sInstance->mState.mainThreadQueueMutex);

		for (auto& func : sInstance->mState.mainThreadQueue)
			func();

		sInstance->mState.mainThreadQueue.clear();
	}

	void Application::Run(int argc, char** argv)
	{
		CreateApplication(argc, argv);

		while (sInstance->mState.running)
		{
			f32 time = Stopwatch::GetTime();
			Timestep timestep = time - sInstance->mState.lastFrameTime;
			sInstance->mState.lastFrameTime = time;

			sInstance->ExecuteMainThread();

			EventManager::Dispatch();

			if (!sInstance->mState.minimised)
			{
				for (Layer* layer : sInstance->mLayerStack)
					layer->onUpdate(timestep);
			}

			{
				imcpp::ImScopedFrame imguiFrame = sInstance->mImGuiHandler->newFrame();
				ImGuizmo::BeginFrame();
				for (Layer* layer : sInstance->mLayerStack)
					layer->onImGuiRender();
			}

			sInstance->mWindow->onUpdate();
		}

		delete sInstance;
	}

	void Application::Close()
	{
		if (!sInstance->mState.blockExit) 
			sInstance->mState.running = false;
	}

	void Application::SubmitActionToMainThread(const Action<>& function)
	{
		std::scoped_lock<std::mutex> lock(sInstance->mState.mainThreadQueueMutex);

		sInstance->mState.mainThreadQueue.emplace_back(function);
	}

	void Application::SendNetMessage(const Message& msg)
	{
		if (!sInstance->mNetworkLayer)
		{
			LAB_CORE_ERROR("Application does not have network capabilities. Add a network layer!");
			return;
		}

		sInstance->mNetworkLayer->send(msg);
	}

	void Application::ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec)
	{
		JsonObj settings = JSON::Open(settingsPath);
		if (settings.empty())
			return;

		if (settings.contains("Startup"))
		{
			auto startup = settings["Startup"];
			if (startup.contains("Fullscreen"))		
				outSpec.fullscreen = startup["Fullscreen"].get<bool>();
			if (startup.contains("WindowWidth"))	
				outSpec.resolution.width = startup["WindowWidth"].get<u32>();
			if (startup.contains("WindowHeight"))	
				outSpec.resolution.height = startup["WindowHeight"].get<u32>();
			if (startup.contains("WorkingDir"))		
				outSpec.workingDir = startup["WorkingDir"].get<fs::path>();
			if (startup.contains("Project"))		
				outSpec.startupProject = startup["Project"].get<fs::path>();

			auto scripting = startup["Scripting"];
			if (scripting.contains("CoreAssemblyPath")) 
				outSpec.scriptConfig.coreAssemblyPath = scripting["CoreAssemblyPath"].get<fs::path>();
		}
	}

	void Application::WriteSettings(const std::filesystem::path& settingsPath)
	{
		JsonObj settingsJSON = JSON::Open(settingsPath);
		if (settingsJSON.empty())
			LAB_CORE_WARN("Settings file did not exist, creating from scratch...");

		const ApplicationSpec& spec = Application::GetSpec();
		JsonObj& startupSettings = settingsJSON["Startup"];

		startupSettings["Fullscreen"] = spec.fullscreen;
		startupSettings["WindowWidth"] = spec.resolution.width;
		startupSettings["WindowHeight"] = spec.resolution.height;
		startupSettings["WorkingDir"] = spec.workingDir;
		startupSettings["Project"] = spec.startupProject;

		JsonObj& scriptSettings = startupSettings["Scripting"];
		scriptSettings["CoreAssemblyPath"] = spec.scriptConfig.coreAssemblyPath;

		JSON::Write(settingsPath, settingsJSON);
	}

	void Application::BlockEsc(bool block)
	{
		sInstance->mState.blockExit = block;
	}

	void Application::BlockEvents(bool block)
	{
		sInstance->mImGuiHandler->blockEvents(block);
	}
}