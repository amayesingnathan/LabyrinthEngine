#include "Lpch.h"
#include "ScriptCache.h"

#include "ScriptEngine.h"

namespace Labyrinth {

	FieldBuffer::FieldBuffer(ScriptFieldType type)
	{
		switch (type)
		{
		case ScriptFieldType::Boolean:	set<bool>(); break;
		case ScriptFieldType::Int8:		set<i8>();; break;
		case ScriptFieldType::Int16:	set<i16>();; break;
		case ScriptFieldType::Int32:	set<i32>();; break;
		case ScriptFieldType::Int64:	set<i64>();; break;
		case ScriptFieldType::UInt8:	set<u8>();; break;
		case ScriptFieldType::UInt16:	set<u16>();; break;
		case ScriptFieldType::UInt32:	set<u32>();; break;
		case ScriptFieldType::UInt64:	set<u64>();; break;
		case ScriptFieldType::Float:	set<f32>();; break;
		case ScriptFieldType::Double:	set<f64>();; break;
		case ScriptFieldType::Vector2:	set<glm::vec2>();; break;
		case ScriptFieldType::Vector3:	set<glm::vec3>();; break;
		case ScriptFieldType::Vector4:	set<glm::vec4>();; break;
		case ScriptFieldType::Entity:	set<UUID>(0);; break;
		default: break;
		}
	}

	FieldInitialiser::FieldInitialiser(const ScriptField& field) :
		type(field.type), value(field.type)
	{
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
			field.value.release();

		sCachedFields.erase(id);
	}

	void ScriptCache::UnsetField(UUID entID, const std::string& fieldName)
	{
		auto& cachedFields = sCachedFields[entID];
		auto& field = cachedFields[fieldName];
		field.value.release();
	}
}