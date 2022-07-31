#pragma once

#include "ScriptObject.h"
#include "ScriptEngineInternal.h"

namespace Labyrinth {

	class Scene;
	class UUID;
	struct ScriptComponent;

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadCoreAssembly(const std::filesystem::path& path);
		static void ReloadCoreAssembly();

		static void OnRuntimeStart(Ref<Scene> context);
		static void OnRuntimeStop();

		static Ref<Scene> GetContext();

		static std::unordered_map<std::string, Ref<ScriptClass>>& GetEntityClasses();
		static Ref<ScriptClass> GetEntityClass(const std::string& className);
		static bool EntityClassExists(const std::string& className);

		static ScriptEngineData* GetData();

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses(MonoAssembly* assembly);
	};
}