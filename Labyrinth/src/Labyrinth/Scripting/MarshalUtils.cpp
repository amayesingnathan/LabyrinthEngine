#include "Lpch.h"
#include "MarshalUtils.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include "ScriptEngineInternal.h"
#include "ScriptUtils.h"

namespace Laby {

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
		return mono_string_new(ScriptEngineInternal::GetAppDomain(), str.c_str());
	}

}