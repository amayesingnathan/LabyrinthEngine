#include "Lpch.h"
#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

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

			mono_field_set_value(mInstance, scriptField->field, field.value);
		}
	}

	void ScriptObject::GetFieldValueInternal(const ScriptField* scriptField, void* result) const
	{
		mono_field_get_value(mInstance, scriptField->field, result);
	}

	void ScriptObject::SetFieldValueInternal(const ScriptField* scriptField, void* fieldVal)
	{
		mono_field_set_value(mInstance, scriptField->field, fieldVal);
	}
}