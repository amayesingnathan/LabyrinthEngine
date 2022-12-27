#include "Lpch.h"
#include "Application.h"

#include <Labyrinth/IO/JSON.h>
#include <Labyrinth/Tools/Utils.h>

namespace Laby {

	Application::Application(const ApplicationSpec& spec)
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		if (!mSpecification.workingDir.empty())
			std::filesystem::current_path(mSpecification.workingDir);

		mWindow = Window::Create(WindowProps(mSpecification.name, mSpecification.resolution.width, mSpecification.resolution.height, mSpecification.fullscreen));
		mWindow->setEventCallback(LAB_BIND_EVENT_FUNC(Application::OnEvent));

		//Renderer::Init();
		//ScriptEngine::Init(spec.scriptConfig);

		mImGuiLayer = new ImGuiLayer();
		PushOverlay(mImGuiLayer);
	}

	Application::~Application()
	{
		for (Layer* layer : mLayerStack)
		{
			layer->onDetach();
			delete layer;
		}
	}
	void Application::Run(int argc, char** argv)
	{
		CreateApplication(argc, argv);

		while (sInstance->mState.running)
		{
			f32 time = Stopwatch::GetTime();
			Timestep timestep = time - sInstance->mState.lastFrameTime;
			sInstance->mState.lastFrameTime = time;

			if (!sInstance->mState.minimised)
			{
				for (Layer* layer : sInstance->mLayerStack)
					layer->onUpdate(timestep);
			}

			sInstance->mImGuiLayer->begin();
			{
				for (Layer* layer : sInstance->mLayerStack)
					layer->onImGuiRender();
			}
			sInstance->mImGuiLayer->end();

			sInstance->mWindow->onUpdate();
		}
	}

	void Application::ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec)
	{
		JsonObj settings = JSON::Open(settingsPath);
		if (settings.empty())
			return;

		if (settings.contains("Startup"))
		{
			auto startup = settings["Startup"];
			if (startup.contains("Fullscreen"))		outSpec.fullscreen = startup["Fullscreen"].get<bool>();
			if (startup.contains("WindowWidth"))	outSpec.resolution.width = startup["WindowWidth"].get<u32>();
			if (startup.contains("WindowHeight"))	outSpec.resolution.height = startup["WindowHeight"].get<u32>();
			if (startup.contains("WorkingDir"))		outSpec.workingDir = startup["WorkingDir"].get<fs::path>();
			if (startup.contains("Project"))		outSpec.startupProject = startup["Project"].get<fs::path>();

			auto scripting = startup["Scripting"];
			if (scripting.contains("CoreAssemblyPath")) outSpec.scriptConfig.coreAssemblyPath = scripting["CoreAssemblyPath"].get<fs::path>();
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
}