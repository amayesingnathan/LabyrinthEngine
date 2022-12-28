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

		virtual EventTypeFlag getListeningEvents() const = 0;
		virtual void onEvent(Event& e) = 0;

		bool accept(EventTypeFlag type) const { return (getListeningEvents() & type); }
	};

#define LISTENING_EVENTS(events)  static constexpr EventTypeFlag GetStaticType() { return events; }\
								  virtual EventTypeFlag getListeningEvents() const override { return GetStaticType(); }
}