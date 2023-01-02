#pragma once

#include "ScriptDefinitions.h"
#include "ScriptObject.h"

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/IO/Filesystem.h>

#define LAB_CORE_ASSEMBLY_INDEX 0
#define LAB_APP_ASSEMBLY_INDEX LAB_CORE_ASSEMBLY_INDEX + 1
#define LAB_MAX_ASSEMBLIES (usize)2

namespace Laby {

	struct ScriptEngineConfig
	{
		fs::path coreAssemblyPath = "resources/Scripts/Labyrinth-ScriptCore.dll";
	};

	struct AssemblyInfo : public RefCounted
	{
		std::filesystem::path filepath = "";
		MonoAssembly* assembly = nullptr;
		MonoImage* assemblyImage = nullptr;
		std::unordered_map<std::string, Ref<ScriptClass>> classes;
		bool isCoreAssembly = false;
	};

	using AssembliesCache = std::array<Ref<AssemblyInfo>, LAB_MAX_ASSEMBLIES>;

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
		MonoDomain* newAppDomain = nullptr;

		AssembliesCache loadedAssemblies;
		ScriptEngineConfig config;

		Ref<ScriptClass> entityClass = nullptr;

		bool initialised = false;
		bool delayedShutdown = false;
	};

	class ScriptEngineInternal
	{
	private:
		static void Init(const ScriptEngineConfig& config);
		static void Shutdown();

		static void InitMono();
		static void ShutdownMono();

		static MonoDomain* GetAppDomain();
		static Ref<AssemblyInfo> GetCoreAssemblyInfo();
		static Ref<AssemblyInfo> GetAppAssemblyInfo();

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
		static bool ReloadAssembly(const std::filesystem::path& assemblyPath);
		static bool LoadCoreAssembly();
		static bool LoadAppAssembly(const std::filesystem::path& assemblyPath);
		static void UnloadAssembly(Ref<AssemblyInfo> assemblyInfo);

		static const ScriptEngineConfig& GetConfig();

		static void LoadAssemblyClasses(Ref<AssemblyInfo> assembly);
		static AssembliesCache& GetLoadedAssemblies();

		static Ref<ScriptClass> GetCoreEntityClass();

		inline static ScriptEngineData* sInternalData = nullptr;

		friend class ScriptEngine;
		friend class ScriptClass;
		friend class ScriptGlue;
		friend class ScriptObject;
		friend class MarshalUtils;
		friend class GlueFunctions;
	};
}