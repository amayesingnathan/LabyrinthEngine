#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include "ScriptUtils.h"
#include "ScriptGlue.h"
#include "ScriptCache.h"
#include "MarshalUtils.h"

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
		Ref<Scene> activeScene = sContext->GetActive();

		activeScene->GetEntitiesWith<ScriptComponent>().each([&](auto e, auto& sc)
		{
			std::string className(sc.className.data());
			if (!scriptClasses.contains(className))
				return;

			Entity entity = { e, activeScene };
			UUID id = entity.GetUUID();
			sc.instance = Ref<ScriptObject>::Create(scriptClasses[className], id);
			sc.initialised = true;
			sc.instance->OnStart();
			sc.instance->SetFieldValues(ScriptCache::GetFields(id));
		});
	}

	void ScriptEngine::OnRuntimeStop()
	{
		sContext->GetActive()->GetEntitiesWith<ScriptComponent>().each([&](auto entity, auto& sc)
		{
			sc.instance.Reset();
			sc.initialised = false;
		});
	}

	void ScriptEngine::SetContext(const Ref<SceneManager>& sceneManager)
	{
		sContext = sceneManager;
		if (sceneManager)
			sContext->AddSceneChangeCallback(ScriptEngine::OnSceneChange);
	}

	void ScriptEngine::OnSceneChange(Ref<Scene> newScene)
	{
		Ref<Scene> activeScene = sContext->GetActive();
		if (activeScene)
		{
			activeScene->GetEntitiesWith<ScriptComponent>().each([&](auto entity, auto& sc)
			{
				sc.instance.Reset();
				sc.initialised = false;
			});
		}

		if (newScene)
		{
			std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;
			newScene->GetEntitiesWith<ScriptComponent>().each([&](auto e, auto& sc)
			{
				std::string className(sc.className.data());
				if (!scriptClasses.contains(className))
					return;

				Entity entity = { e, sContext };
				UUID id = entity.GetUUID();
				sc.instance = Ref<ScriptObject>::Create(scriptClasses[className], id);
				sc.initialised = true;
				sc.instance->OnStart();
				sc.instance->SetFieldValues(ScriptCache::GetFields(id));
			});
		}
	}

	void ScriptEngine::LoadAppAssembly()
	{
		ScriptEngineInternal::LoadAppAssembly(Project::GetScriptModuleFilePath());
	}

	void ScriptEngine::ReloadAssembly()
	{
		auto appAssembly = ScriptEngineInternal::GetAppAssemblyInfo();
		LAB_CORE_INFO("[ScriptEngine] Reloading {0}", appAssembly->filepath);
		LAB_CORE_ASSERT(sContext, "No scene manager context!");
		Ref<Scene> scene = sContext->GetActive();

		if (scene)
		{
			scene->GetEntitiesWith<ScriptComponent>().each([=](auto entity, auto& sc)
			{
				sc.instance.Reset();
				sc.initialised = false;
			});
		}

		ScriptEngineInternal::LoadAppAssembly(appAssembly->filepath);

		if (scene)
		{
			std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;

			scene->GetEntitiesWith<ScriptComponent>().each([&](auto e, auto& sc)
			{
				std::string className(sc.className.data());
				if (!scriptClasses.contains(className))
					return;

				Entity entity = { e, scene };
				UUID id = entity.GetUUID();
				sc.instance = Ref<ScriptObject>::Create(scriptClasses[className], id);
				sc.initialised = true;
				sc.instance->OnStart();
				sc.instance->SetFieldValues(ScriptCache::GetFields(id));
			});
		}
	}

	void ScriptEngine::UnloadAppAssembly()
	{
		if (!sContext)
			return;

		Ref<Scene> scene = sContext->GetActive();
		if (!scene)
			return;

		scene->GetEntitiesWith<ScriptComponent>().each([=](auto entity, auto& sc)
		{
			sc.instance.Reset();
			sc.initialised = false;
		});

		ScriptEngineInternal::UnloadAssembly(ScriptEngineInternal::GetAppAssemblyInfo());
	}

	void ScriptEngine::RegenScriptProject()
	{
		fs::path rootDirectory = FileUtils::LabRoot();
		std::string envVarString = fmt::format("LAB_ROOT_DIR={}", rootDirectory.string());
		System::SetEnv(envVarString);

		fs::path batchFilePath = Project::GetProjectDirectory() / "create-script-projects";
		System::Call(batchFilePath.string());
	}

	Ref<ScriptClass> ScriptEngine::GetAppClass(std::string_view name)
	{
		const auto& classes = ScriptEngineInternal::GetAppAssemblyInfo()->classes;
		std::string className(name.data());
		if (!classes.contains(className))
			return nullptr;

		return classes.at(className);
	}

	std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetAppClasses() { return ScriptEngineInternal::GetAppAssemblyInfo()->classes; }

	Ref<ScriptObject> ScriptEngine::GetScriptInstance(UUID id)
	{
		if (!sContext)
			return nullptr;

		Entity entity = sContext->GetActive()->FindEntity(id);
		if (!entity.HasComponent<ScriptComponent>())
			return nullptr;

		return entity.GetComponent<ScriptComponent>().instance;
	}

}