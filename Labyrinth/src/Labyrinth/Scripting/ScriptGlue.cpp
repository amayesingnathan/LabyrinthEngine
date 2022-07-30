#include "Lpch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"

#include <Labyrinth/IO/Input.h>

#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include "mono/metadata/object.h"

#include <glm/glm.hpp>

namespace Labyrinth {

#define LAB_ADD_INTERNAL_CALL(funcName) mono_add_internal_call("Labyrinth.Internal::" #funcName, funcName)

#pragma region LoggingFunctions

	static void Log_Trace(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_TRACE(cStr);
		mono_free(cStr);
	}
	static void Log_Info(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_INFO(cStr);
		mono_free(cStr);
	}
	static void Log_Warning(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_WARN(cStr);
		mono_free(cStr);
	}
	static void Log_Error(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_ERROR(cStr);
		mono_free(cStr);
	}
	static void Log_Critical(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_CRITICAL(cStr);
		mono_free(cStr);
	}
#pragma endregion

#pragma region InputFunctions
	static bool Input_IsKeyPressed(KeyCode key)
	{
		return Input::IsKeyPressed(key);
	}
#pragma endregion

#pragma region ComponentFunctions
	static void Entity_GetTranslation(UUID id, glm::vec3* outTranslation)
	{
		Ref<Scene> context = ScriptEngine::GetContext();
		Entity entity = context->findEntity(id);

		*outTranslation = entity.getComponent<TransformComponent>().translation;
	}
	static void Entity_SetTranslation(UUID id, glm::vec3* translation)
	{
		Ref<Scene> context = ScriptEngine::GetContext();
		Entity entity = context->findEntity(id);

		entity.getComponent<TransformComponent>().translation = *translation;
	}
#pragma endregion

	void ScriptGlue::RegisterFunctions()
	{
#pragma region LoggingMacros
		LAB_ADD_INTERNAL_CALL(Log_Trace);
		LAB_ADD_INTERNAL_CALL(Log_Info);
		LAB_ADD_INTERNAL_CALL(Log_Warning);
		LAB_ADD_INTERNAL_CALL(Log_Error);
		LAB_ADD_INTERNAL_CALL(Log_Critical);
#pragma endregion

#pragma region InputMacros
		LAB_ADD_INTERNAL_CALL(Input_IsKeyPressed);
#pragma endregion

#pragma region ComponentMacros
		LAB_ADD_INTERNAL_CALL(Entity_GetTranslation);
		LAB_ADD_INTERNAL_CALL(Entity_SetTranslation);
#pragma endregion
	}

}