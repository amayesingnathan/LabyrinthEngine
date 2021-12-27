#include "Lpch.h"

#include "ECS/Systems/ScriptEngine.h"

#include "ECS/Components/GameComponents.h"

void ScriptEngine::update()
{
	//Use updated transform to update sprite position
	auto scripts = mScene->mRegistry.view<ScriptComponent>();

	std::for_each(std::execution::par, scripts.begin(), scripts.end(), [&scripts](const auto entity) {
		auto& script = scripts.get<ScriptComponent>(entity);
		if (!script.instance)
		{
			script.InstantiateFunc();
			script.OnCreateFunc(script.instance);
		}

		script.OnUpdateFunc(script.instance);
	});
}
