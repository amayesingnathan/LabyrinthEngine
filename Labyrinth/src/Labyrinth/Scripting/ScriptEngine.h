#pragma once

#include "Labyrinth/Events/Event.h"

namespace Labyrinth {

	class Scene;

	class ScriptEngine
	{
	public:
		ScriptEngine(const Ref<Scene>& scene)
			: mScene(scene) {}
		~ScriptEngine() = default;

		void onUpdate();
		void onEvent(Event& e);

	private:
		void executeScript();

	private:
		Ref<Scene> mScene;
	};

}