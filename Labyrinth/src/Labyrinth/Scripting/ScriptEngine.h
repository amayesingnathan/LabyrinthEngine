#pragma once

#include <Labyrinth/Scene/SceneManager.h>

#include "ScriptEngineInternal.h"
#include "ScriptObject.h"
#include "ScriptClass.h"

namespace Laby {

	class ScriptEngine
	{
	public:
		static void Init(const ScriptEngineConfig& config);
		static void Shutdown();

		static void OnRuntimeStart();
		static void OnRuntimeStop();

		static void SetContext(const Ref<SceneManager>& scene);
		static Ref<SceneManager> GetContext() { return sContext; }
		static void OnSceneChange(Ref<Scene> newScene);

		static void LoadAppAssembly();
		static void ReloadAssembly();
		static void UnloadAppAssembly();

		static void RegenScriptProject();

		static Ref<ScriptClass> GetEntityClass() { return ScriptEngineInternal::GetCoreEntityClass(); }
		static Ref<ScriptClass> GetAppClass(std::string_view name);
		static std::unordered_map<std::string, Ref<ScriptClass>>& GetAppClasses();
		static Ref<ScriptObject> GetScriptInstance(UUID id);

	private:
		inline static Ref<SceneManager> sContext = nullptr;
	};
}