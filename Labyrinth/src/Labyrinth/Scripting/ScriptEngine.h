#pragma once

#include <Labyrinth/Scene/Scene.h>

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

		static void SetContext(const Ref<Scene>& scene) { sContext = scene; }
		static Ref<Scene> GetContext() { return sContext; }

		static void LoadAppAssembly();
		static void ReloadAssembly(const std::filesystem::path& assemblyPath);
		static void UnloadAppAssembly();

		static Ref<ScriptClass> GetEntityClass() { return ScriptEngineInternal::GetCoreEntityClass(); }
		static Ref<ScriptClass> GetAppClass(const std::string& name);
		static std::unordered_map<std::string, Ref<ScriptClass>>& GetAppClasses();
		static Ref<ScriptObject> GetScriptInstance(UUID id);

	private:
		inline static Ref<Scene> sContext = nullptr;
	};
}