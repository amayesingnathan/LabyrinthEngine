#pragma once

#include <Labyrinth/Events/IEventListener.h>

#include "ImGuiCpp.h"

namespace Laby {

	class ImGuiHandler : public imcpp::ImHandler, public IEventListener
	{
	public:
		LISTENING_EVENTS(EVENT_CATEGORY_MOUSE, EVENT_CATEGORY_KEY)

		ImGuiHandler();

		void onEvent(Event& e);

		void blockEvents(bool block) { mBlockEvents = block; }

	private:
		bool mBlockEvents;
	};
}