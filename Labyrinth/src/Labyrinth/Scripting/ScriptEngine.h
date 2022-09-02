#pragma once

#include "ScriptEngineInternal.h"
#include "ScriptObject.h"

namespace Labyrinth {

	class Scene;

	class ScriptEngine
	{
	public:
		static void Init(const ScriptEngineConfig& config);
		static void Shutdown();

		static void OnRuntimeStart();
		static void OnRuntimeStop();

		static void SetContext(const Ref<Scene>& scene);
		static Ref<Scene> GetContext();

		static void LoadAppAssembly();
		static void ReloadAssembly(const std::filesystem::path& assemblyPath);
		static void UnloadAppAssembly();

		static std::unordered_map<std::string, Ref<ScriptClass>>& GetAppClasses();
		static Ref<ScriptObject> GetScriptInstance(UUID id);
	};
}