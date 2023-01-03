#include "Lpch.h"
#include "Tilemap.h"

namespace Laby {

	Tilemap::Tilemap(const std::string& name, usize width, usize height)
		: mName(name), mTexture(width, height), mBehaviour(width, height)
	{
	}
}