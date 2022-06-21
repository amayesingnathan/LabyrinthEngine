#pragma once

#include <Labyrinth/Core/System/Assert.h>

#include <random>

namespace Labyrinth {

	class Random
	{
	public:
		static float Float(float low = 0.0f, float high = 1.0f)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<float> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static double Double(double low = 0.0, double high = 1.0)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<double> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static int Int(int low = 0, int high = 1)
		{
			LAB_CORE_ASSERT(low < high, "min is >= max");
			std::uniform_real_distribution<int> dist(low, high);
			auto& mt = Get();
			return dist(mt);
		}
		static bool Bool()
		{
			std::uniform_real_distribution<int> dist(0.5);
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