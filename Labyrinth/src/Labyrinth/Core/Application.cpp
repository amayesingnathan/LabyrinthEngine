#include "Lpch.h"
#include "Application.h"

#include <Labyrinth/IO/JSON.h>
#include <Labyrinth/Networking/ServerLayer.h>
#include <Labyrinth/Networking/ClientLayer.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Tools/Timing.h>

namespace Laby {

	Application::Application(ApplicationSpec* spec)
		: slc::Application(spec)
	{
		if (!spec->workingDir.empty())
			std::filesystem::current_path(spec->workingDir);

		RegisterSystem<Renderer>();
		RegisterSystem<ScriptEngine>(spec->scriptConfig);
	}

	void Application::SendNetMessage(const Message& msg)
	{
		if (!sInstance->mNetworkLayer)
		{
			LAB_CORE_ERROR("Application does not have network capabilities. Add a network layer!");
			return;
		}

		sInstance->mNetworkLayer->Send(msg);
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

		const ApplicationSpec& spec = Application::GetSpec<ApplicationSpec>();
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