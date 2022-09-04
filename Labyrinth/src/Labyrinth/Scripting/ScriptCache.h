#pragma once

#include "FieldTypes.h"
#include "ScriptClass.h"

namespace Labyrinth {

	struct FieldInitialiser
	{
		ScriptFieldType type = ScriptFieldType::None;
		void* value = nullptr;

		FieldInitialiser() = default;
		FieldInitialiser(const ScriptField& f);
	};

	using FieldValues = std::unordered_map<std::string, FieldInitialiser>;

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
			FieldInitialiser& init = sCachedFields[id][field];
			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(init.field.type), "Template parameter is not a valid type for this field!");
			init.value = fieldVal;
		}
		static void UnsetField(UUID entID, const std::string& field);

		static FieldValues& GetFields(UUID entID) { return sCachedFields[entID]; }

		template<typename T>
		static T* GetFieldValue(UUID entID, const std::string& fieldName)
		{
			auto& cachedFields = sCachedFields[entID];
			auto& fieldValue = cachedFields[fieldName];

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(fieldValue.type), "Template parameter is not a valid type for this field!");
			return (T*)fieldValue.value;
		}

	private:
		inline static std::unordered_map<UUID, FieldValues> sCachedFields;
	};
}