#pragma once

#include "ScriptFwd.h"
#include "ScriptClass.h"

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Scene/Scene.h>

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
}