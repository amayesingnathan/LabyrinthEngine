#include "Lpch.h"
#include "ScriptEngine.h"

#include "Scripting.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;
	};

	static ScriptEngineData* sData = nullptr;

	void ScriptEngine::Init()
	{
		sData = new ScriptEngineData();

		InitMono();
		LoadCoreAssembly("Resources/Scripts/Labyrinth-ScriptCore.dll");

		ScriptClass testclass("Labyrinth", "TestClass");
		MonoObject* instance = testclass.instantiate();
		testclass.invokeMethod(instance, "PrintMessage");
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
	}

	MonoObject* ScriptClass::instantiate()
	{
		return Scripting::InstantiateClass(sData->appDomain, mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, size_t argc)
	{
		return mono_class_get_method_from_name(mMonoClass, name.c_str(), argc);
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, size_t argc)
	{
		return Scripting::InstantiateClassInternal(sData->appDomain, mMonoClass, argv, argc);
	}


}