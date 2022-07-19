#include "Lpch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"

namespace Labyrinth {

#define LAB_ADD_INTERNAL_CALL(funcName) mono_add_internal_call("Labyrinth.Internal::" #funcName, funcName)

#pragma region Logging

	static void LogTrace(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_TRACE(cStr);
		mono_free(cStr);
	}
	static void LogInfo(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_INFO(cStr);
		mono_free(cStr);
	}
	static void LogWarning(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_WARN(cStr);
		mono_free(cStr);
	}
	static void LogError(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_ERROR(cStr);
		mono_free(cStr);
	}
	static void LogCritical(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_CRITICAL(cStr);
		mono_free(cStr);
	}
#pragma endregion

	void ScriptGlue::RegisterFunctions()
	{
#pragma region Logging
		LAB_ADD_INTERNAL_CALL(LogTrace);
		LAB_ADD_INTERNAL_CALL(LogInfo);
		LAB_ADD_INTERNAL_CALL(LogWarning);
		LAB_ADD_INTERNAL_CALL(LogError);
		LAB_ADD_INTERNAL_CALL(LogCritical);
#pragma endregion

	}

}