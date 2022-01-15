#include "Lpch.h"
#include "ScriptEngine.h"

#include "Labyrinth/Scene/Scene.h"

#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	void ScriptEngine::onUpdate()
	{
		//Use updated transform to update sprite position
		auto scripts = mScene->mRegistry.view<NativeScriptComponent>();

		for (auto entity : scripts)
		{
		}
	}

	void ScriptEngine::onEvent(Event& e)
	{
		enum class EventType
		{
			None = -1,
			WindowClose, WindowResize, WindowFocus, WindowLostFoucus, WindowMoved,
			AppTick, AppUpdate, AppRender,
			KeyPressed, KeyReleased, KeyTyped,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
		};

		LAB_PROFILE_FUNCTION();
		mScene->mRegistry.view<ScriptTriggerComponent>().each([&](auto entityID, auto& stc)
		{
			switch (stc.trigger.type)
			{
				case Labyrinth::EventType::MouseButtonPressed:
				{

				}
			}
			dispatcher.dispatch<stc.trigger>(LAB_BIND_EVENT_FUNC(Application::OnWindowClose));
		});
	}

}