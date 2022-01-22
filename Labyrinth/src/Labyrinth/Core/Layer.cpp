#include "Lpch.h"
#include "Layer.h"

namespace Labyrinth {

	Layer::Layer(const std::string& name) 
#ifdef LAB_DEBUG
		: mDebugName(name)
#endif
	{
	}

}