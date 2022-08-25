#pragma once

namespace Labyrinth {

	class Timestep
	{
	public:
		Timestep(f32 time = 0.0f)
			: mTime(time)
		{
		}

		operator f32() const { return mTime; }

		f32 getSeconds() const { return mTime; }
		f32 getMilliseconds() const { return mTime * 1000.f; }

	private:
		f32 mTime;
	};

}