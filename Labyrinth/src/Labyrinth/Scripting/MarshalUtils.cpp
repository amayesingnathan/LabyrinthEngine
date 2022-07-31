#include "Lpch.h"
#include "MarshalUtils.h"

#include "ScriptEngine.h"
#include "ScriptUtils.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

namespace Labyrinth {

	std::string MarshalUtils::MonoStringToUTF8(MonoString* monoString)
	{
		if (monoString == nullptr || mono_string_length(monoString) == 0)
			return "";

		MonoError error;
		char* utf8 = mono_string_to_utf8_checked(monoString, &error);
		if (ScriptUtils::CheckMonoError(error))
			return "";
		std::string result(utf8);
		mono_free(utf8);
		return result;
	}

	MonoString* MarshalUtils::UTF8StringToMono(const std::string& str)
	{
		return mono_string_new(ScriptEngine::GetData()->appDomain, str.c_str());
	}

}