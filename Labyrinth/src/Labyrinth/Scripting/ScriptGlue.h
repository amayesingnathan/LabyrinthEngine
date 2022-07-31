#pragma once

namespace Labyrinth {

	class ScriptGlue
	{
	public:
		static void Register();

	private:
		static void RegisterComponentTypes();
		static void RegisterInternalCalls();
	};

}