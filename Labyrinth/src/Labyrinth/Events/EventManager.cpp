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
				if (listener->accept(e.type))
					listener->onEvent(e);

				if (e.handled)
					break;
			}

			sEventQueue.pop();
		}
	}
}