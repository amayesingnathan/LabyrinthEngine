#include "Lpch.h"
#include "ScriptCache.h"

#include "ScriptEngine.h"

namespace Labyrinth {

	FieldValue::FieldValue(const ScriptField& f) :
		field(f)
	{
		switch (field.type)
		{
		case ScriptFieldType::Boolean:	value = new bool(); break;
		case ScriptFieldType::Int8:		value = new i8(); break;
		case ScriptFieldType::Int16:	value = new i16(); break;
		case ScriptFieldType::Int32:	value = new i32(); break;
		case ScriptFieldType::Int64:	value = new i64(); break;
		case ScriptFieldType::UInt8:	value = new u8(); break;
		case ScriptFieldType::UInt16:	value = new u16(); break;
		case ScriptFieldType::UInt32:	value = new u32(); break;
		case ScriptFieldType::UInt64:	value = new u64(); break;
		case ScriptFieldType::Float:	value = new f32(); break;
		case ScriptFieldType::Double:	value = new f64(); break;
		case ScriptFieldType::Vector2:	value = new glm::vec2(); break;
		case ScriptFieldType::Vector3:	value = new glm::vec3(); break;
		case ScriptFieldType::Vector4:	value = new glm::vec4(); break;
		}
	}

	void ScriptCache::RegisterEntity(UUID id, const std::string& scriptClass)
	{
		auto& cachedFields = sCachedFields[id];
		for (const ScriptField& field : ScriptEngine::GetAppClass(scriptClass)->getFields())
			cachedFields.try_emplace(field.name, field);
	}

	void ScriptCache::RegisterEntity(UUID id, Ref<ScriptClass> scriptClass)
	{
		auto& cachedFields = sCachedFields[id];
		for (const ScriptField& field : scriptClass->getFields())
			cachedFields.try_emplace(field.name, field);
	}

	void ScriptCache::RemoveEntity(UUID id)
	{
		auto& cachedFields = sCachedFields[id];
		for (auto& [fieldName, field] : cachedFields)
		{
			delete field.value;
			field.value = nullptr;
		}
		sCachedFields.erase(id);
	}

	void ScriptCache::UnsetField(UUID entID, const std::string& fieldName)
	{
		auto& cachedFields = sCachedFields[entID];
		auto& field = cachedFields[fieldName];
		delete field.value;
		field.value = nullptr;
	}
}