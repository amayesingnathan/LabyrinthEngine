#include "Lpch.h"
#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "ScriptEngine.h"

namespace Laby{

	void ScriptObject::setFieldValues(const FieldValues& fieldVals)
	{
		if (!mInstance)
			return;

		for (auto& [name, field] : fieldVals)
		{
			const ScriptField* scriptField = mClass->getField(name);
			if (!scriptField)
			{
				LAB_CORE_WARN("Cached field name did not match class field name!");
				continue;
			}

			if (!field.value)
				continue;

			void* data = (void*)field.value.data();
			Ref<ScriptObject> entity = nullptr;
			if (field.type == ScriptFieldType::Entity)
			{
				entity = Ref<ScriptObject>::Create(ScriptEngine::GetEntityClass(), field.value.read<UUID>());
				data = entity->obj();
			}

			mono_field_set_value(mInstance, scriptField->field, data);
		}
	}

	void ScriptObject::getEntityFieldValue(std::string_view fieldName, UUID& id) const
	{
		const ScriptField* scriptField = mClass->getField(fieldName);
		if (!scriptField)
			return;

		LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<UUID>(scriptField->type), "Template type is not valid for this field type!");

		MonoObject* obj = mono_field_get_value_object(mono_object_get_domain(mInstance), scriptField->field, mInstance);
		if (!obj)
			return;

		MonoClassField* idField = ScriptEngine::GetEntityClass()->getField("ID")->field;
		if (!idField)
			return;

		mono_field_get_value(obj, idField, &id);
	}

	void ScriptObject::GetFieldValueInternal(const ScriptField* scriptField, void* result) const
	{
		mono_field_get_value(mInstance, scriptField->field, result);
	}

	void ScriptObject::SetFieldValueInternal(const ScriptField* scriptField, const void* fieldVal)
	{
		mono_field_set_value(mInstance, scriptField->field, (void*)fieldVal);
	}
}