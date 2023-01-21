#include "Lpch.h"
#include "Ref.h"

#include "Assert.h"

namespace Laby {

	bool RefTracker::IsTracked(void* data)
	{
		LAB_CORE_ASSERT(data, "Data is null!");
		return sRefSet.count(data) != 0;
	}

	void RefTracker::AddToTrackedRefs(void* data)
	{
		LAB_CORE_ASSERT(data);
		sRefSet.insert(data);
	}

	void RefTracker::RemoveFromTrackedRefs(void* data)
	{
		LAB_CORE_ASSERT(data, "Data is null!");
		LAB_CORE_ASSERT(sRefSet.count(data) != 0, "Ref not being tracked!");
		sRefSet.erase(data);
	}
}