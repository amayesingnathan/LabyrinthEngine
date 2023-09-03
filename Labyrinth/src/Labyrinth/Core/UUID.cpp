#include "Lpch.h"
#include "UUID.h"

#include <random>

namespace Laby {

	static std::random_device sRandomDevice;
	static std::mt19937_64 sEngine(sRandomDevice());
	static std::uniform_int_distribution<u64>(sUniformDistribution);

	UUID::UUID()
		: mID(sUniformDistribution(sEngine))
	{
	}
}