#include "Lpch.h"
#include "Groups.h"

namespace Laby::ECS {

	Groups::Groups(Ref<Scene> context)
	{
		mGroupTuple = MakeSingle<AllGroups::TupleType>(GenerateTuple(context));
	}
}