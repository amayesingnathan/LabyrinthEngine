#pragma once

#include "ScriptFwd.h"

namespace Labyrinth {

	class MarshalUtils
	{
	public:
		static std::string MonoStringToUTF8(MonoString* monoString);
		static MonoString* UTF8StringToMono(const std::string& str);
	};

}