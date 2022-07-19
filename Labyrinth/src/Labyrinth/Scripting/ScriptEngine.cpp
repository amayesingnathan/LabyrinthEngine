#include "Lpch.h"
#include "ScriptEngine.h"

#include "Scripting.h"
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
	};

	static ScriptEngineData* sData = nullptr;

	void ScriptEngine::Init()
	{
		sData = new ScriptEngineData();

		InitMono();
		LoadCoreAssembly("Resources/Scripts/Labyrinth-ScriptCore.dll");

		ScriptGlue::RegisterFunctions();

		sData->entityClass = ScriptClass::Create("Labyrinth", "Entity");
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

		sData->coreAssembly = Scripting::LoadMonoAssembly(path);
		sData->coreAssemblyImage = mono_assembly_get_image(sData->coreAssembly);

#ifdef LAB_DEBUG
		Scripting::PrintAssemblyTypes(sData->coreAssembly);
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
		mMonoClass = mono_object_get_class(instance);
		mClassNamespace = mono_class_get_namespace(mMonoClass);
		mClassName = mono_class_get_name(mMonoClass);

		void* iter = nullptr;
		while (MonoMethod* method = mono_class_get_methods(mMonoClass, &iter))
			mMethods.emplace(mono_method_get_name(method), method);
	}

	MonoObject* ScriptClass::instantiate() const
	{
		return Scripting::InstantiateClass(sData->appDomain, mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name)
	{
		LAB_CORE_ASSERT(mMethods.count(name) != 0, "Method must exist on class!");
		return mMethods.at(name);
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, int argc)
	{
		return Scripting::InstantiateClassInternal(sData->appDomain, mMonoClass, argv, argc);
	}


}