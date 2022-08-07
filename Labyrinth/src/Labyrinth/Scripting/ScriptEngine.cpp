#include "Lpch.h"
#include "ScriptEngine.h"

#include "ScriptUtils.h"
#include "ScriptGlue.h"
#include "MarshalUtils.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	struct ScriptEngineState
	{
		Ref<Scene> context = nullptr;
	};

	static ScriptEngineState* sData = nullptr;

	void ScriptEngine::Init(const ScriptEngineConfig& config)
	{
		sData = new ScriptEngineState();

		ScriptEngineInternal::Init(config);
	}

	void ScriptEngine::Shutdown()
	{
		Ref<Scene> scene = sData->context;

		if (!scene)
			return;

		ScriptEngineInternal::Shutdown();

		delete sData;
		sData = nullptr;
	}

	void ScriptEngine::OnRuntimeStart()
	{
		LAB_PROFILE_FUNCTION();
		LAB_CORE_ASSERT(sData->context, "Tring to initialize script runtime without setting the scene context!");

		std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;

		auto scene = sData->context;
		scene->getEntitiesWith<ScriptComponent>().each([&](auto entity, auto& sc)
		{
			Entity e = { entity, scene };
			if (scriptClasses.count(sc.className) != 0)
			{
				sc.instance = ScriptObject::Create(scriptClasses[sc.className], e.getUUID());
				sc.initialised = true;
				sc.instance->onStart();
			}
		});
	}

	void ScriptEngine::OnRuntimeStop()
	{
		sData->context->getEntitiesWith<ScriptComponent>().each([&](auto& sc)
		{
			sc.instance = nullptr;
			sc.initialised = false;
		});
	}

	void ScriptEngine::SetContext(const Ref<Scene>& scene) { sData->context = scene; }
	Ref<Scene> ScriptEngine::GetContext() { return sData->context; }

	void ScriptEngine::LoadAppAssembly()
	{
		ScriptEngineInternal::LoadAppAssembly(Project::GetScriptModuleFilePath());
	}

	void ScriptEngine::ReloadAssembly(const std::filesystem::path& assemblyPath)
	{
		LAB_PROFILE_FUNCTION();
		LAB_CORE_INFO("[ScriptEngine] Reloading {0}", assemblyPath);

		auto scene = sData->context;
		if (scene)
		{
			scene->getEntitiesWith<ScriptComponent>().each([=](auto& sc)
			{
				sc.instance = nullptr;
				sc.initialised = false;
			});
		}

		ScriptEngineInternal::LoadAppAssembly(assemblyPath);

		if (scene)
		{
			std::unordered_map<std::string, Ref<ScriptClass>>& scriptClasses = ScriptEngineInternal::GetAppAssemblyInfo()->classes;
			auto scene = sData->context;
			scene->getEntitiesWith<ScriptComponent>().each([&](auto entity, auto& sc)
			{
				Entity e = { entity, scene };
				if (scriptClasses.count(sc.className) != 0)
				{
					sc.instance = ScriptObject::Create(scriptClasses[sc.className], e.getUUID());
					sc.initialised = true;
					sc.instance->onStart();
				}
			});
		}
	}

	void ScriptEngine::UnloadAppAssembly()
	{
		auto scene = sData->context;

		if (!scene)
			return;

		scene->getEntitiesWith<ScriptComponent>().each([=](auto& sc)
		{
			sc.instance = nullptr;
			sc.initialised = false;
		});
		ScriptEngineInternal::UnloadAssembly(ScriptEngineInternal::GetAppAssemblyInfo());
	}

	std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetAppClasses() { return ScriptEngineInternal::GetAppAssemblyInfo()->classes; }
}