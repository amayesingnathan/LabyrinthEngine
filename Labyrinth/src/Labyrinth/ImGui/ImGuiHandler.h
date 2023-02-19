#pragma once

#include <Labyrinth/Events/IEventListener.h>

#include "ImGuiCpp.h"

namespace Laby {

	class ImGuiHandler : public imcpp::ImHandler, public IEventListener
	{
	public:
		ImGuiHandler();

		void onEvent(Event& e);
		LISTENING_EVENTS(EVENT_CATEGORY_MOUSE, EVENT_CATEGORY_KEY)

		void blockEvents(bool block) { mBlockEvents = block; }

	private:
		bool mBlockEvents = false;
	};
}