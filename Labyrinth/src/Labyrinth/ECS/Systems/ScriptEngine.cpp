#include "Lpch.h"

#if 0

#include "./ScriptEngine.h"

#include "Scene.h"

#include "ECS/Components/GameComponents.h"

/*
	Contains all scripts for the engine. All scripts defined here must have explicit template declaration
	for ScriptEngine::addScript at the bottom of this translation unit to be added to an entity.
*/
#include "ECS/Systems/Scripts.cpp"

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
	std::cout << "Script Engine cleaned.\n";
}

template<typename T>
void ScriptEngine::addScript(Entity* entity)
{
	entity->addComponent<ScriptComponent>().bind<T>();
}

//Must keep explicit declarations up to date with all available scripts
template void ScriptEngine::addScript<TestScript>(Entity* entity);

#endif