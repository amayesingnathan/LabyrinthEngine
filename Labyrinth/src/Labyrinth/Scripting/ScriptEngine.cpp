#include "Lpch.h"
#include "ScriptEngine.h"

#include "ScriptUtils.h"
#include "ScriptGlue.h"
#include "ScriptClass.h"
#include "ScriptObject.h"

#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;
		std::filesystem::path assemblyPath;

		Ref<ScriptClass> entityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> entityClasses;

		Ref<Scene> context;
	};

	static ScriptEngineData* sData = nullptr;

	void ScriptEngine::Init()
	{
		sData = new ScriptEngineData();

		InitMono();
		LoadCoreAssembly("Resources/Scripts/Labyrinth-ScriptCore.dll");
		LoadAssemblyClasses(sData->coreAssembly);

		ScriptGlue::RegisterFunctions();
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();

		delete sData;
		sData = nullptr;
	}

	void ScriptEngine::LoadCoreAssembly(const std::filesystem::path& path)
	{
		sData->appDomain = mono_domain_create_appdomain("LabScriptRuntime", nullptr);
		mono_domain_set(sData->appDomain, true);

		sData->coreAssembly = ScriptUtils::LoadMonoAssembly(path);
		sData->coreAssemblyImage = mono_assembly_get_image(sData->coreAssembly);

		sData->assemblyPath = path;

		sData->entityClass = ScriptClass::Create("Labyrinth", "Entity");

#ifdef LAB_DEBUG
		ScriptUtils::PrintAssemblyTypes(sData->coreAssembly);
#endif
	}

	void ScriptEngine::ReloadCoreAssembly()
	{
		sData->coreAssembly = ScriptUtils::LoadMonoAssembly(sData->assemblyPath);
		sData->coreAssemblyImage = mono_assembly_get_image(sData->coreAssembly);

		sData->entityClass = ScriptClass::Create("Labyrinth", "Entity");

#ifdef LAB_DEBUG
		ScriptUtils::PrintAssemblyTypes(sData->coreAssembly);
#endif

		LoadAssemblyClasses(sData->coreAssembly);

		ScriptGlue::RegisterFunctions();
	}

	void ScriptEngine::OnRuntimeStart(Ref<Scene> context)
	{
		sData->context = context;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		sData->context = nullptr;
	}

	Ref<Scene> ScriptEngine::GetContext()
	{
		return sData->context;
	}

	std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetEntityClasses()
	{
		return sData->entityClasses;
	}

	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& className)
	{
		LAB_CORE_ASSERT(sData->entityClasses.count(className) != 0);

		return sData->entityClasses[className];
	}

	bool ScriptEngine::EntityClassExists(const std::string& className)
	{
		return sData->entityClasses.count(className) != 0;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("LabJITRuntime");
		LAB_CORE_ASSERT(rootDomain, "Mono Runtime was not initialised!");

		// Store the root domain pointer
		sData->rootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// mono_domain_unload(s_Data->AppDomain);
		sData->appDomain = nullptr;

		// mono_jit_cleanup(s_Data->RootDomain);
		sData->rootDomain = nullptr;
	}

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		sData->entityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* entityClass = mono_class_from_name(image, "Labyrinth", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			std::string nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			std::string name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string fullname;

			if (nameSpace.empty())
				fullname = name;
			else
				fullname = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass = mono_class_from_name(image, nameSpace.c_str(), name.c_str());

			if (!monoClass || monoClass == entityClass)
				continue;

			if (mono_class_is_subclass_of(monoClass, entityClass, false))
				sData->entityClasses[fullname] = ScriptClass::Create(monoClass);
		}
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: mClassNamespace(classNamespace), mClassName(className)
	{
		mMonoClass = mono_class_from_name(sData->coreAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	ScriptClass::ScriptClass(MonoClass* klass)
		: mClassNamespace(mono_class_get_namespace(klass)),mClassName(mono_class_get_name(klass)), mMonoClass(klass)
	{
	}

	ScriptClass::ScriptClass(MonoObject* instance)
	{
		if (!instance) return;

		mMonoClass = mono_object_get_class(instance);
		mClassNamespace = mono_class_get_namespace(mMonoClass);
		mClassName = mono_class_get_name(mMonoClass);
	}

	MonoObject* ScriptClass::instantiate() const
	{
		return ScriptUtils::InstantiateClass(sData->appDomain, mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int argc)
	{
		return mono_class_get_method_from_name(mMonoClass, name.c_str(), argc);
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, int argc)
	{
		MonoMethod* constructor = sData->entityClass->getMethod(".ctor", 1);
		return ScriptUtils::InstantiateClassInternal(sData->appDomain, mMonoClass, constructor, argv);
	}
}