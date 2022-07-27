#include "Lpch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"

#include <Labyrinth/Scene/Scene.h>

#include "mono/metadata/object.h"

#include <glm/glm.hpp>

namespace Labyrinth {

#define LAB_ADD_INTERNAL_CALL(funcName) mono_add_internal_call("Labyrinth.Internal::" #funcName, funcName)

#pragma region LoggingFunctions

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

#pragma region ComponentFunctions
	static void Entity_GetTranslation(UUID* id, glm::vec3* outTranslation)
	{
		Ref<Scene> context = ScriptEngine::GetContext();
	}
	static void Entity_SetTranslation(UUID* id, glm::vec3* outTranslation)
	{
		Ref<Scene> context = ScriptEngine::GetContext();
	}
#pragma endregion

	void ScriptGlue::RegisterFunctions()
	{
#pragma region LoggingMacros
		LAB_ADD_INTERNAL_CALL(LogTrace);
		LAB_ADD_INTERNAL_CALL(LogInfo);
		LAB_ADD_INTERNAL_CALL(LogWarning);
		LAB_ADD_INTERNAL_CALL(LogError);
		LAB_ADD_INTERNAL_CALL(LogCritical);
#pragma endregion
#pragma region ComponentMacros
		LAB_ADD_INTERNAL_CALL(Entity_GetTranslation);
		LAB_ADD_INTERNAL_CALL(Entity_SetTranslation);
#pragma endregion
	}

}