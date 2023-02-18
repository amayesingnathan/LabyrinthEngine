#include "Lpch.h"
#include "EventManager.h"

#include <Labyrinth/Core/Application.h>

#include "IEventListener.h"

namespace Laby {

	void EventManager::Dispatch()
	{
		while (!sEventQueue.empty())
		{
			Event& e = sEventQueue.front();

			// Handle imgui events first
			if (sImGuiListener->accept(e.type))
				sImGuiListener->onEvent(e);

			auto filteredListeners = std::views::common(sListeners) | 
				std::views::filter([=](IEventListener* listener) { return listener->accept(e.type); });

			for (IEventListener* listener : filteredListeners)
			{
				if (e.handled)
					break;
				listener->onEvent(e);
			}

			sEventQueue.pop();
		}
	}

	void EventManager::RegisterListener(IEventListener* listener, ListenerType type)
	{
		switch (type)
		{
		case ListenerType::Generic:
		{
			// Some events may create a new listener while we're iterating through the listeners, so postpone addition till start of new game loop.
			Application::SubmitActionToMainThread([&, listener]() { sListeners.emplace_back(listener); });
			break;
		}
		case ListenerType::App:
		{
			sListeners.emplace_back(listener);
			break;
		}
		case ListenerType::ImGui:
		{
			sImGuiListener = listener;
			break;
		}
		}
	}

	void EventManager::DeregisterListener(IEventListener* listener)
	{
		Application::SubmitActionToMainThread([&, listener]() { std::erase(sListeners, listener); });
	}
}