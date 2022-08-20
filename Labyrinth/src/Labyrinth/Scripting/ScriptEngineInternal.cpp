#include "Lpch.h"
#include "ScriptEngineInternal.h"

#include "ScriptGlue.h"

#include <Labyrinth/IO/Filesystem.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	void ScriptEngineInternal::Init(const ScriptEngineConfig& config)
	{
		LAB_CORE_ASSERT(!sInternalData, "Trying to initialize Script Engine multiple times!");
		sInternalData= new ScriptEngineData();
		sInternalData->config = config;

		InitMono();

		sInternalData->loadedAssemblies[LAB_CORE_ASSEMBLY_INDEX] = Ref<AssemblyInfo>::Create();
		sInternalData->loadedAssemblies[LAB_APP_ASSEMBLY_INDEX] = Ref<AssemblyInfo>::Create();

		LoadCoreAssembly();
		LAB_CORE_ASSERT(sInternalData->loadedAssemblies[LAB_CORE_ASSEMBLY_INDEX]->assembly, "Failed to load Laabyrinth script core!");
	}

	void ScriptEngineInternal::Shutdown()
	{
		ShutdownMono();

		delete sInternalData;
		sInternalData = nullptr;
	}

	void ScriptEngineInternal::InitMono()
	{
		if (sInternalData->initialised)
			return;

		mono_set_assemblies_path("mono/lib");

		sInternalData->rootDomain = mono_jit_init("LabJITRuntime");
		LAB_CORE_ASSERT(sInternalData->rootDomain, "Mono Runtime was not initialised!");

		sInternalData->initialised = true;
		LAB_CORE_INFO("[ScriptEngine] Initialized Mono");
	}

	void ScriptEngineInternal::ShutdownMono()
	{
		if (!sInternalData->initialised)
		{
			LAB_CORE_WARN("[ScriptEngine] Trying to shutdown Mono multiple times!");
			return;
		}

		sInternalData->appDomain = nullptr;
		mono_jit_cleanup(sInternalData->rootDomain);
		sInternalData->rootDomain = nullptr;

		sInternalData->initialised = false;
	}

	MonoDomain* ScriptEngineInternal::GetAppDomain() { return sInternalData->appDomain; }

	Ref<AssemblyInfo> ScriptEngineInternal::GetCoreAssemblyInfo()
	{
		LAB_CORE_ASSERT(sInternalData);
		LAB_CORE_ASSERT(sInternalData->initialised);
		return sInternalData->loadedAssemblies[LAB_CORE_ASSEMBLY_INDEX];
	}

	Ref<AssemblyInfo> ScriptEngineInternal::GetAppAssemblyInfo()
	{
		LAB_CORE_ASSERT(sInternalData);
		LAB_CORE_ASSERT(sInternalData->initialised);
		return sInternalData->loadedAssemblies[LAB_APP_ASSEMBLY_INDEX];
	}

	MonoAssembly* ScriptEngineInternal::LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		Buffer fileData = FileUtils::Read(assemblyPath);

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData.as<char>(), (u32)fileData.size, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			LAB_CORE_ERROR("[ScriptEngine] Failed to open C# assembly '{0}'\n\t\tMessage: {1}", assemblyPath, errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
		mono_image_close(image);
		return assembly;
	}

	bool ScriptEngineInternal::ReloadAssembly(const std::filesystem::path& assemblyPath)
	{
		if (!LoadCoreAssembly())
			return false;

		if (!LoadAppAssembly(assemblyPath))
			return false;

		return true;
	}

	bool ScriptEngineInternal::LoadCoreAssembly()
	{
		if (sInternalData->appDomain)
		{
			sInternalData->newAppDomain = mono_domain_create_appdomain("LabScriptRuntime", nullptr);
			mono_domain_set(sInternalData->newAppDomain, true);
			sInternalData->delayedShutdown = true;
		}
		else
		{
			sInternalData->appDomain = mono_domain_create_appdomain("LabScriptRuntime", nullptr);
			mono_domain_set(sInternalData->appDomain, true);
			sInternalData->delayedShutdown = false;
		}

		auto coreAssemblyInfo = sInternalData->loadedAssemblies[LAB_CORE_ASSEMBLY_INDEX];
		coreAssemblyInfo->assembly = LoadMonoAssembly(sInternalData->config.coreAssemblyPath);

		if (coreAssemblyInfo->assembly == nullptr)
		{
			LAB_CORE_ERROR("[ScriptEngine] Failed to load core assembly!");
			return false;
		}

		coreAssemblyInfo->classes.clear();
		coreAssemblyInfo->assemblyImage = mono_assembly_get_image(coreAssemblyInfo->assembly);
		coreAssemblyInfo->isCoreAssembly = true;
		LAB_CORE_INFO("[ScriptEngine] Successfully loaded core assembly from: {0}", sInternalData->config.coreAssemblyPath);

		sInternalData->entityClass = Ref<ScriptClass>::Create(mono_class_from_name(coreAssemblyInfo->assemblyImage, "Labyrinth", "Entity"));
		LAB_CORE_ASSERT(sInternalData->entityClass->valid())

#ifdef LAB_DEBUG
		ScriptUtils::PrintAssemblyTypes(coreAssemblyInfo->assembly);
#endif

		return true;
	}

	bool ScriptEngineInternal::LoadAppAssembly(const std::filesystem::path& assemblyPath)
	{
		if (!std::filesystem::exists(assemblyPath))
			return false;

		auto appAssemblyInfo = sInternalData->loadedAssemblies[LAB_APP_ASSEMBLY_INDEX];

		if (appAssemblyInfo->assembly)
		{
			appAssemblyInfo->assembly = nullptr;
			appAssemblyInfo->assemblyImage = nullptr;
			return ReloadAssembly(assemblyPath);
		}

		auto appAssembly = LoadMonoAssembly(assemblyPath);

		if (appAssembly == nullptr)
		{
			LAB_CORE_ERROR("[ScriptEngine] Failed to load app assembly!");
			return false;
		}

		appAssemblyInfo->assembly = appAssembly;
		appAssemblyInfo->assemblyImage = mono_assembly_get_image(appAssemblyInfo->assembly);
		appAssemblyInfo->classes.clear();
		appAssemblyInfo->isCoreAssembly = false;

		ScriptGlue::Register();
		
		LoadAssemblyClasses(appAssemblyInfo);

		if (sInternalData->delayedShutdown)
		{
			mono_domain_unload(sInternalData->appDomain);

			sInternalData->appDomain = sInternalData->newAppDomain;
			sInternalData->newAppDomain = nullptr;
			sInternalData->delayedShutdown = false;
		}

		return true;
	}

	void ScriptEngineInternal::UnloadAssembly(Ref<AssemblyInfo> assemblyInfo)
	{
		assemblyInfo->classes.clear();
		assemblyInfo->assembly = nullptr;
		assemblyInfo->assemblyImage = nullptr;

		if (assemblyInfo->isCoreAssembly)
			sInternalData->loadedAssemblies[LAB_CORE_ASSEMBLY_INDEX] = Ref<AssemblyInfo>::Create();
		else
			sInternalData->loadedAssemblies[LAB_APP_ASSEMBLY_INDEX] = Ref<AssemblyInfo>::Create();
	}

	const ScriptEngineConfig& ScriptEngineInternal::GetConfig() { return sInternalData->config; }

	void ScriptEngineInternal::LoadAssemblyClasses(Ref<AssemblyInfo> assembly)
	{
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(assembly->assemblyImage, MONO_TABLE_TYPEDEF);
		i32 numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (i32 i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			std::string nameSpace = mono_metadata_string_heap(assembly->assemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			std::string name = mono_metadata_string_heap(assembly->assemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullname;

			if (nameSpace.empty())
				fullname = name;
			else
				fullname = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass = mono_class_from_name(assembly->assemblyImage, nameSpace.c_str(), name.c_str());

			if (!monoClass)
				continue;

			if (mono_class_is_subclass_of(monoClass, *sInternalData->entityClass, false))
				assembly->classes[fullname] = ScriptClass::Create(monoClass);
		}
	}

	AssembliesCache& ScriptEngineInternal::GetLoadedAssemblies()
	{
		LAB_CORE_ASSERT(sInternalData);
		LAB_CORE_ASSERT(sInternalData->initialised);
		return sInternalData->loadedAssemblies;
	}

	Ref<ScriptClass> ScriptEngineInternal::GetCoreEntityClass()
	{
		return sInternalData->entityClass;
	}

}