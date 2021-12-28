#include "Lpch.h"

#include "ECS/Systems/ScriptEngine.h"

#include "ECS/Components/GameComponents.h"

void ScriptEngine::update()
{
	//Use updated transform to update sprite position
	auto scripts = mScene->mRegistry.view<ScriptComponent>();

	for (auto entity : scripts)
	{
		auto& script = scripts.get<ScriptComponent>(entity);
		if (!script.instance)
		{
			script.instance = script.InstantiateScript();
			script.instance->mEntity = Entity{ entity, mScene };
			script.OnCreateFunc(script.instance);
		}

		script.OnUpdateFunc(script.instance);
	}
}

void ScriptEngine::clean()
{
	//Use updated transform to update sprite position
	auto scripts = mScene->mRegistry.view<ScriptComponent>();

	for (auto entity : scripts)
	{
		auto& script = scripts.get<ScriptComponent>(entity);
		if (script.instance)
		{
			script.OnDestroyFunc(script.instance);
			script.DestroyScript(&script);
		}
	}
	std::cout << "Script Engine cleaned\n";
}