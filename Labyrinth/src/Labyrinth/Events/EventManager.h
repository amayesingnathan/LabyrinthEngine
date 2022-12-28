#pragma once

#include <queue>

#include "Event.h"

namespace Laby {

	class IEventListener;

	class EventManager
	{
	public:
		static void RegisterListener(IEventListener* listener) { sListeners.emplace_back(listener); }
		static void Post(const Event& event) { sEventQueue.emplace(event); }

		static void Dispatch();

	private:
		inline static std::vector<IEventListener*> sListeners;
		inline static std::queue<Event> sEventQueue;
	};
}