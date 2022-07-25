#include "Lpch.h"
#include "ScriptEngine.h"

#include "ScriptUtils.h"
#include "ScriptGlue.h"
#include "ScriptClass.h"
#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		Ref<ScriptClass> entityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> entityClasses;
	};

	static ScriptEngineData* sData = nullptr;

	void ScriptEngine::Init()
	{
		sData = new ScriptEngineData();

		InitMono();
		LoadCoreAssembly("Resources/Scripts/Labyrinth-ScriptCore.dll");
		LoadAssemblyClasses(sData->coreAssembly);

		ScriptGlue::RegisterFunctions();

		sData->entityClass = ScriptClass::Create("Labyrinth", "Main");
		ScriptObject instance(sData->entityClass);
		ScriptObject instance2(sData->entityClass, 2.4f);
		instance.invokeMethod("PrintMessage");
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

#ifdef LAB_DEBUG
		ScriptUtils::PrintAssemblyTypes(sData->coreAssembly);
#endif
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
			if (mono_class_is_subclass_of(monoClass, entityClass, false))
				sData->entityClasses[fullname] = ScriptClass::Create(monoClass);
		}
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: mClassNamespace(classNamespace), mClassName(className)
	{
		mMonoClass = mono_class_from_name(sData->coreAssemblyImage, classNamespace.c_str(), className.c_str());

		void* iter = nullptr;
		while (MonoMethod* method = mono_class_get_methods(mMonoClass, &iter))
			mMethods.emplace(mono_method_get_name(method), method);
	}

	ScriptClass::ScriptClass(MonoClass* klass)
		: mClassNamespace(mono_class_get_namespace(klass)),mClassName(mono_class_get_name(klass)), mMonoClass(klass)
	{
		void* iter = nullptr;
		while (MonoMethod* method = mono_class_get_methods(mMonoClass, &iter))
			mMethods.emplace(mono_method_get_name(method), method);
	}

	ScriptClass::ScriptClass(MonoObject* instance)
	{
		if (!instance) return;

		mMonoClass = mono_object_get_class(instance);
		mClassNamespace = mono_class_get_namespace(mMonoClass);
		mClassName = mono_class_get_name(mMonoClass);

		void* iter = nullptr;
		while (MonoMethod* method = mono_class_get_methods(mMonoClass, &iter))
			mMethods.emplace(mono_method_get_name(method), method);
	}

	MonoObject* ScriptClass::instantiate() const
	{
		return ScriptUtils::InstantiateClass(sData->appDomain, mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name)
	{
		LAB_CORE_ASSERT(mMethods.count(name) != 0, "Method must exist on class!");
		return mMethods.at(name);
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, int argc)
	{
		return ScriptUtils::InstantiateClassInternal(sData->appDomain, mMonoClass, argv, argc);
	}


}