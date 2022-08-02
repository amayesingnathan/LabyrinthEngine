#pragma once

#include "ScriptObject.h"

namespace Labyrinth {

	class Scene;

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnRuntimeStart();
		static void OnRuntimeStop();

		static void SetContext(const Ref<Scene>& scene);
		static Ref<Scene> GetContext();

		static void LoadAppAssembly();
		static void ReloadAssembly(const std::filesystem::path& assemblyPath);
		static void UnloadAppAssembly();

		static std::unordered_map<std::string, Ref<ScriptClass>>& GetAppClasses();
	};
}