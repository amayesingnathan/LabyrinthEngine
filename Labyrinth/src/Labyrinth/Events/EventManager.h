#pragma once

#include <queue>

#include "Event.h"

namespace Laby {

	class IEventListener;

	enum class ListenerType
	{
		Generic,
		App,
		ImGui
	};

	class EventManager
	{
	public:
		template<IsEvent TEvent, typename... TArgs>
		static void Post(TArgs&&... args)
		{
			Event& e = sEventQueue.emplace();
			e.init<TEvent>(std::forward<TArgs>(args)...);
		}

		static void Dispatch();
		
	private:
		static void RegisterListener(IEventListener* listener, ListenerType type = ListenerType::Generic);
		static void DeregisterListener(IEventListener* listener);

	private:
		inline static IEventListener* sImGuiListener = nullptr;
		inline static std::vector<IEventListener*> sListeners;
		inline static std::queue<Event> sEventQueue;

		friend IEventListener;
	};
}