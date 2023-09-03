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
		static void RegisterListener(IEventListener* listener, ListenerType type);
		static void DeregisterListener(IEventListener* listener, ListenerType type);

	private:
		inline static IEventListener* sAppListener = nullptr;
		inline static IEventListener* sImGuiListener = nullptr;
		inline static std::vector<IEventListener*> sGenericListeners;

		inline static std::queue<Event> sEventQueue;

		friend IEventListener;
	};
}