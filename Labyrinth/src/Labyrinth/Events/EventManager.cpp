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

			// Handle app events first
			if (sAppListener->accept(e.type))
				sAppListener->onEvent(e);

			// Handle imgui events next
			if (sImGuiListener->accept(e.type))
				sImGuiListener->onEvent(e);

			auto filteredListeners = std::views::common(sGenericListeners) |
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
			Application::SubmitActionToMainThread([&, listener]() { sGenericListeners.emplace_back(listener); });
			break;
		}
		case ListenerType::App:
		{
			sAppListener = listener;
			break;
		}
		case ListenerType::ImGui:
		{
			sImGuiListener = listener;
			break;
		}
		}
	}

	void EventManager::DeregisterListener(IEventListener* listener, ListenerType type)
	{
		switch (type)
		{
		case ListenerType::Generic:
		{
			Application::SubmitActionToMainThread([&, listener]() { std::erase(sGenericListeners, listener); });
			break;
		}
		case ListenerType::App:
		{
			sAppListener = nullptr;
			break;
		}
		case ListenerType::ImGui:
		{
			sImGuiListener = nullptr;
			break;
		}
		}
	}
}