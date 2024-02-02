#include "Lpch.h"
#include "ScriptField.h"

#include "ScriptEngine.h"

namespace Laby {

	FieldBuffer::FieldBuffer(ScriptFieldType type)
	{
		switch (type)
		{
		case ScriptFieldType::Boolean:	Set<bool>(); break;
		case ScriptFieldType::Int8:		Set<i8>();; break;
		case ScriptFieldType::Int16:	Set<i16>();; break;
		case ScriptFieldType::Int32:	Set<i32>();; break;
		case ScriptFieldType::Int64:	Set<i64>();; break;
		case ScriptFieldType::UInt8:	Set<u8>();; break;
		case ScriptFieldType::UInt16:	Set<u16>();; break;
		case ScriptFieldType::UInt32:	Set<u32>();; break;
		case ScriptFieldType::UInt64:	Set<u64>();; break;
		case ScriptFieldType::Float:	Set<f32>();; break;
		case ScriptFieldType::Double:	Set<f64>();; break;
		case ScriptFieldType::Vector2:	Set<glm::vec2>();; break;
		case ScriptFieldType::Vector3:	Set<glm::vec3>();; break;
		case ScriptFieldType::Vector4:	Set<glm::vec4>();; break;
		case ScriptFieldType::Entity:	Set<UUID>(0);; break;
		default: break;
		}
	}

	FieldInitialiser::FieldInitialiser(const ScriptField& field) :
		type(field.type), value(field.type)
	{
	}
}