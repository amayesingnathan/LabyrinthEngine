#include "Lpch.h"
#include "Application.h"

#include <Labyrinth/Core/System/Log.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/IO/JSON.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Tools/PlatformUtils.h>

namespace Labyrinth {

	Application* Application::sInstance = nullptr;

	Application::Application(const ApplicationSpec& spec)
		: mSpecification(spec)
	{
		LAB_PROFILE_FUNCTION();

		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		if (!mSpecification.workingDir.empty())
			std::filesystem::current_path(mSpecification.workingDir);

		mWindow = Window::Create(WindowProps(mSpecification.name, mSpecification.resolution.width, mSpecification.resolution.height, mSpecification.fullscreen));
		mWindow->setEventCallback(LAB_BIND_EVENT_FUNC(Application::onEvent));

		Renderer::Init();
		ScriptEngine::Init(spec.scriptConfig);

		mImGuiLayer = new ImGuiLayer();
		pushOverlay(mImGuiLayer);

	}

	Application::~Application()
	{
		LAB_PROFILE_FUNCTION();

		for (Layer* layer : mLayerStack)
		{
			layer->onDetach();
			delete layer;
		}

		ScriptEngine::Shutdown();
		Renderer::Shutdown();
		Project::SetActive(nullptr);
	}

	void Application::Close()
	{
		Application& app = Get();
		if (app.mBlockExit) return;

		app.mRunning = false;
	}

	void Application::run()
	{
		LAB_PROFILE_FUNCTION();

		while (mRunning)
		{
			LAB_PROFILE_SCOPE("RunLoop");

			f32 time = Stopwatch::GetTime();
			Timestep timestep = time - mLastFrameTime;
			mLastFrameTime = time;

			if (!mMinimised)
			{
				LAB_PROFILE_SCOPE("LayerStack Update");

				for (Layer* layer : mLayerStack)
					layer->onUpdate(timestep);
			}

			mImGuiLayer->begin();
			{
				LAB_PROFILE_SCOPE("LayerStack ImGuiRender");

				for (Layer* layer : mLayerStack)
					layer->onImGuiRender();
			}
			mImGuiLayer->end();

			mWindow->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(LAB_BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(Application::OnWindowResize));

		for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); it++)
		{
			if (e.handled)
				break;
			(*it)->onEvent(e);
		}

		// Check for Esc after in case an event in a layer handles it.
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(Application::OnKeyPressed));
	}

	void Application::pushLayer(Layer* layer)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Application::Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		LAB_PROFILE_FUNCTION();

		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			mMinimised = true;
			return false;
		}

		mMinimised = false;
		Renderer::OnWindowResize(e.getWidth(), e.getHeight());
		return false;
	}
	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.getKeyCode() == Key::Escape)
			Application::Close();

		return false;
	}

	void Application::ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec)
	{
		JsonObj settings = JSON::Open(settingsPath);
		if (settings.empty())
			return;

		if (settings.contains("Startup"))
		{
			auto startup = settings["Startup"];
			if (startup.contains("Fullscreen")) outSpec.fullscreen = startup["Fullscreen"].get<bool>();
			if (startup.contains("WindowWidth")) outSpec.resolution.width = startup["WindowWidth"].get<u32>();
			if (startup.contains("WindowHeight")) outSpec.resolution.height = startup["WindowHeight"].get<u32>();
			if (startup.contains("WorkingDir")) outSpec.workingDir = startup["WorkingDir"].get<fs::path>();
			if (startup.contains("Project")) outSpec.startupProject = startup["Project"].get<fs::path>();

			auto scripting = startup["Scripting"];
			if (scripting.contains("CoreAssemblyPath")) outSpec.scriptConfig.coreAssemblyPath = scripting["CoreAssemblyPath"].get<fs::path>();
		}
	}

	void Application::WriteSettings(const std::filesystem::path& settingsPath)
	{
		JsonObj settingsJSON = JSON::Open(settingsPath);
		if (settingsJSON.empty())
			LAB_CORE_WARN("Settings file did not exist, creating from scratch...");

		const ApplicationSpec& spec = Application::Get().getSpec();
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
