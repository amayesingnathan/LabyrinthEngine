#include "Lpch.h"
#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	void ScriptObject::GetFieldValueInternal(const ScriptField* scriptField, void* result)
	{
		mono_field_get_value(mInstance, scriptField->field, result);
	}

	void ScriptObject::SetFieldValueInternal(const ScriptField* scriptField, void* fieldVal)
	{
		mono_field_set_value(mInstance, scriptField->field, fieldVal);
	}
}