#include "Lpch.h"
#include "ScriptCache.h"

#include "ScriptEngine.h"

namespace Laby {

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
			field.value.clear();

		sCachedFields.erase(id);
	}

	void ScriptCache::UnsetField(UUID entID, const std::string& fieldName)
	{
		auto& cachedFields = sCachedFields[entID];
		auto& field = cachedFields[fieldName];
		field.value.clear();
	}
}