#pragma once

#include "ScriptField.h"
#include "ScriptClass.h"

namespace Laby {

	class ScriptCache
	{
	public:
		static void RegisterEntity(UUID id, std::string_view scriptClass);
		static void RegisterEntity(UUID id, Ref<ScriptClass> scriptClass);
		static void RemoveEntity(UUID id);

		template<IsFieldType T>
		static void SetField(UUID entID, const std::string& field, const T& val)
		{
			FieldInitialiser& init = sCachedFields[entID][field];
			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(init.type), "Template parameter is not a valid type for this field!");
			init.value.set(val);
		}
		static void UnsetField(UUID entID, const std::string& field);

		static FieldValues& GetFields(UUID entID) { return sCachedFields[entID]; }

		template<IsFieldType T>
		static T& GetFieldValue(UUID entID, const std::string& fieldName)
		{
			auto& cachedFields = sCachedFields[entID];
			auto& fieldValue = cachedFields[fieldName];

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(fieldValue.type), "Template parameter is not a valid type for this field!");
			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(fieldValue.value), "Field buffer does not contain value!");

			return fieldValue.value.get<T>();
		}

	private:
		inline static std::unordered_map<UUID, FieldValues> sCachedFields;
	};
}