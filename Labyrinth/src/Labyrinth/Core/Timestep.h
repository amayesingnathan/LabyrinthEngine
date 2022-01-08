#pragma once

namespace Labyrinth {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: mTime(time)
		{
		}

		operator float() const { return mTime / 1000.f; }

		float getSeconds() const { return mTime / 1000.f; }
		float getMilliseconds() const { return mTime; }

	private:
		float mTime;
	};

}