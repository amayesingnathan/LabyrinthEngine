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
	};

	static ScriptEngineData* sData = nullptr;

	void ScriptEngine::Init()
	{
		sData = new ScriptEngineData();
		InitMono();
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();

		delete sData;
		sData = nullptr;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("LabJITRuntime");
		LAB_CORE_ASSERT(rootDomain, "Mono Runtime was not initialised!");

		// Store the root domain pointer
		sData->rootDomain = rootDomain;

		sData->appDomain = mono_domain_create_appdomain("LabScriptRuntime", nullptr);
		mono_domain_set(sData->appDomain, true);

		sData->coreAssembly = Scripting::LoadCSharpAssembly("Resources/Scripts/Labyrinth-ScriptCore.dll");
		Scripting::PrintAssemblyTypes(sData->coreAssembly);

		MonoObject* instance = Scripting::InstantiateClass(sData->appDomain, sData->coreAssembly, "Labyrinth", "TestClass");
		MonoString* monoString = mono_string_new(sData->appDomain, "Hello World from C++!");
		void* param = monoString;
		Scripting::CallMethod(instance, "PrintMessageCustom", &param, 1);
	}

	void ScriptEngine::ShutdownMono()
	{
		// mono_domain_unload(s_Data->AppDomain);
		sData->appDomain = nullptr;

		// mono_jit_cleanup(s_Data->RootDomain);
		sData->rootDomain = nullptr;
	}



}