#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "ScriptUtils.h"
#include "ScriptGlue.h"
#include "ScriptCache.h"
#include "MarshalUtils.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

namespace Laby {

	void ScriptEngine::Init(const ScriptEngineConfig& config)
	{
		ScriptEngineInternal::Init(config);
	}

	void ScriptEngine::Shutdown()
	{
		ScriptEngineInternal::Shutdown();
	}

	void ScriptEngine::OnRuntimeStart()
	{
		LAB_CORE_ASSERT(sContext, "Tring to initialize script runtime without setting the scene context!");

		std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;

		sContext->getEntitiesWith<ScriptComponent>().each([&](auto e, auto& sc)
		{
			std::string className(sc.className.data());
			if (!scriptClasses.contains(className))
				return;

			Entity entity = { e, sContext };
			UUID id = entity.getUUID();
			sc.instance = Ref<ScriptObject>::Create(scriptClasses[className], id);
			sc.initialised = true;
			sc.instance->onStart();
			sc.instance->setFieldValues(ScriptCache::GetFields(id));
		});
	}

	void ScriptEngine::OnRuntimeStop()
	{
		sContext->getEntitiesWith<ScriptComponent>().each([&](auto entity, auto& sc)
		{
			sc.instance.reset();
			sc.initialised = false;
		});
	}

	void ScriptEngine::LoadAppAssembly()
	{
		ScriptEngineInternal::LoadAppAssembly(Project::GetScriptModuleFilePath());
	}

	void ScriptEngine::ReloadAssembly()
	{
		auto appAssembly = ScriptEngineInternal::GetAppAssemblyInfo();
		LAB_CORE_INFO("[ScriptEngine] Reloading {0}", appAssembly->filepath);

		if (sContext)
		{
			sContext->getEntitiesWith<ScriptComponent>().each([=](auto entity, auto& sc)
			{
				sc.instance.reset();
				sc.initialised = false;
			});
		}

		ScriptEngineInternal::LoadAppAssembly(appAssembly->filepath);

		if (sContext)
		{
			std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;
			sContext->getEntitiesWith<ScriptComponent>().each([&](auto e, auto& sc)
			{
				std::string className(sc.className.data());
				if (!scriptClasses.contains(className))
					return;

				Entity entity = { e, sContext };
				UUID id = entity.getUUID();
				sc.instance = Ref<ScriptObject>::Create(scriptClasses[className], id);
				sc.initialised = true;
				sc.instance->onStart();
				sc.instance->setFieldValues(ScriptCache::GetFields(id));
			});
		}
	}

	void ScriptEngine::UnloadAppAssembly()
	{
		if (!sContext)
			return;

		sContext->getEntitiesWith<ScriptComponent>().each([=](auto entity, auto& sc)
		{
			sc.instance.reset();
			sc.initialised = false;
		});

		ScriptEngineInternal::UnloadAssembly(ScriptEngineInternal::GetAppAssemblyInfo());
	}

	void ScriptEngine::RegenScriptProject()
	{
		static std::string sEnvVarString;

		fs::path rootDirectory = std::filesystem::absolute("./resources").parent_path();
		while (rootDirectory.stem().string() != "LabyrinthEngine")
			rootDirectory = rootDirectory.parent_path();
		sEnvVarString = "LAB_ROOT_DIR=" + rootDirectory.string();

		fs::path batchFilePath = Project::GetProjectDirectory();
		batchFilePath /= "create-script-projects";

		std::string batchPathString = batchFilePath.string();

#ifdef LAB_PLATFORM_WINDOWS
		int error = _putenv(sEnvVarString.c_str());
		batchPathString += ".bat";
		std::replace(batchPathString.begin(), batchPathString.end(), '/', '\\'); // Only windows
#elif defined(LAB_PLATFORM_LINUX)
		batchPathString += ".sh";
		int error = putenv(sEnvVarString.c_str());
#else
		int error = -1;
#endif
		if (error)
		{
			LAB_CORE_ERROR("Could not set the Labyrinth root directory!");
			return;
		}
		system(batchPathString.c_str());
	}

	Ref<ScriptClass> ScriptEngine::GetAppClass(const std::string& name)
	{
		const auto& classes = ScriptEngineInternal::GetAppAssemblyInfo()->classes;
		if (!classes.contains(name))
			return nullptr;

		return classes.at(name);
	}

	std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetAppClasses() { return ScriptEngineInternal::GetAppAssemblyInfo()->classes; }

	Ref<ScriptObject> ScriptEngine::GetScriptInstance(UUID id)
	{
		if (!sContext)
			return nullptr;

		Entity entity = sContext->findEntity(id);
		if (!entity.hasComponent<ScriptComponent>())
			return nullptr;

		return entity.getComponent<ScriptComponent>().instance;
	}

}