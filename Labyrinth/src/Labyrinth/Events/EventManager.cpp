#include "Lpch.h"
#include "EventManager.h"

#include "IEventListener.h"

namespace Laby {

	void EventManager::Dispatch()
	{
		while (!sEventQueue.empty())
		{
			Event& e = sEventQueue.front();

			for (IEventListener* listener : sListeners)
			{
				if (!e.handled && listener->accept(e.type))
					listener->onEvent(e);
			}

			sEventQueue.pop();
		}
	}
}