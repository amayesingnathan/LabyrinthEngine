#pragma once

#include <Labyrinth/Core/System/Assert.h>

#include <random>

namespace Labyrinth {

	class Random
	{
	public:
		static f32 f32(f32 low = 0.0f, f32 high = 1.0f)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<f32> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static f64 Double(f64 low = 0.0, f64 high = 1.0)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<f64> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static i32 i32(i32 low = 0, i32 high = 1)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<i32> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static bool Bool()
		{
			std::uniform_real_distribution<i32> dist(0.5);
			auto& mt = Get();
			return dist(mt);
		}

	private:
		static std::mt19937 Get()
		{
			static std::random_device seed;
			static std::mt19937 engine(seed());
			return engine;
		}
	};

}