#pragma once

#include "EventManager.h"

namespace Laby {

	class IEventListener
	{
	public:
		IEventListener() { EventManager::RegisterListener(this, mType); }
		virtual ~IEventListener() { EventManager::DeregisterListener(this, mType); }

		virtual constexpr EventTypeFlag getListeningEvents() const = 0;
		virtual void onEvent(Event& e) = 0;
		virtual void setEventCondition(Predicate<>&& condition) { mAcceptCondition = std::move(condition); }

		bool accept(EventTypeFlag type) const
		{ 
			bool acceptType = (getListeningEvents() & type);
			return acceptType && mAcceptCondition();
		}

	private:
		IEventListener(ListenerType type)
			: mType(type)
		{ 
			EventManager::RegisterListener(this, mType);
		}

		friend class Application;
		friend class ImGuiHandler;

	private:
		ListenerType mType = ListenerType::Generic;
		Predicate<> mAcceptCondition = [](){ return true; };
	};

#define LISTENING_EVENTS(...)	static constexpr EventTypeFlag GetStaticType() { return EXPAND_EVENTS(__VA_ARGS__); }\
								virtual constexpr EventTypeFlag getListeningEvents() const override { return GetStaticType(); }
}