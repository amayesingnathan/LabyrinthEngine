#include "Lpch.h"
#include "Groups.h"

namespace Laby::ECS {

	Groups::Groups(Scene* context)
		: mGroupTuple(GenerateTuple(context))
	{
	}
}