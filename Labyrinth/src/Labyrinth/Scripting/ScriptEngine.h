#pragma once

#include "Scripting.h"

namespace Labyrinth {

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadCoreAssembly(const std::filesystem::path& path);

	private:
		static void InitMono();
		static void ShutdownMono();
	};
}