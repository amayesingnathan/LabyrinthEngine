#pragma once

#include "FieldTypes.h"
#include "ScriptClass.h"

namespace Labyrinth {

	struct FieldValue
	{
		ScriptField field;
		void* value = nullptr;

		FieldValue() = default;
		FieldValue(const ScriptField& f);
	};

	using FieldValues = std::unordered_map<std::string, FieldValue>;

	class ScriptCache
	{
	public:
		static void RegisterEntity(UUID id, const std::string& scriptClass);
		static void RegisterEntity(UUID id, Ref<ScriptClass> scriptClass);
		static void RemoveEntity(UUID id);

		template<typename T>
		static void SetField(UUID entID, const std::string& field, const T& val)
		{
			T* fieldVal = new T(val);
			sCachedFields[id][field].value = fieldVal;
		}
		static void UnsetField(UUID entID, const std::string& field);

		static FieldValues& GetFields(UUID entID) { return sCachedFields[entID]; }

		template<typename T>
		static T* GetFieldValue(UUID entID, const std::string& fieldName)
		{
			auto& cachedFields = sCachedFields[entID];
			auto& fieldValue = cachedFields[fieldName];

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(fieldValue.field.type), "Template parameter is not a valid type for this field!");
			return (T*)fieldValue.value;
		}

	private:
		inline static std::unordered_map<UUID, FieldValues> sCachedFields;
	};
}