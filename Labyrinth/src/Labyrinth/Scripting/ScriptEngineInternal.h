#pragma once

#include <Labyrinth/IO/Filesystem.h>

namespace Laby {

	struct ScriptEngineConfig
	{
		fs::path coreAssemblyPath = "resources/Scripts/Labyrinth-ScriptCore.dll";
	};
}