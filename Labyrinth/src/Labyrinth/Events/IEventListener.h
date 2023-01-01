#pragma once

#include "EventManager.h"

namespace Laby {

	class IEventListener
	{
	public:
		IEventListener()
		{
			EventManager::RegisterListener(this);
		}

		virtual constexpr EventTypeFlag getListeningEvents() const = 0;
		virtual void onEvent(Event& e) = 0;

		bool accept(EventTypeFlag type) const { return (getListeningEvents() & type); }
	};

#define LISTENING_EVENTS(...)	static constexpr EventTypeFlag GetStaticType() { return EXPAND_EVENTS(__VA_ARGS__); }\
								virtual constexpr EventTypeFlag getListeningEvents() const override { return GetStaticType(); }
}