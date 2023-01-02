#pragma once

#include "ScriptDefinitions.h"

namespace Laby {

	class MarshalUtils
	{
	public:
		static std::string MonoStringToUTF8(MonoString* monoString);
		static MonoString* UTF8StringToMono(const std::string& str);
	};

}