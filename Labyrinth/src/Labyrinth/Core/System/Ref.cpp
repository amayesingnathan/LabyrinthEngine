#include "Lpch.h"
#include "Ref.h"

namespace Labyrinth {

	void RefTracker::AddToTrackedRefs(void* data)
	{
		LAB_CORE_ASSERT(data);
		sRefSet.insert(data);
	}

	void RefTracker::RemoveFromTrackedRefs(void* data)
	{
		LAB_CORE_ASSERT(data);
		LAB_CORE_ASSERT(sRefSet.count(data) != 0);
		sRefSet.erase(data);
	}

	bool RefTracker::IsTracked(void* data)
	{
		LAB_CORE_ASSERT(data);
		return sRefSet.count(data) != 0;
	}

}