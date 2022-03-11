#pragma once

#include "GameComponents.h"

namespace Labyrinth {

	class Player
	{
	public:
		Player() = default;
		Player(Colour side, bool isCPU = false)
			: mSide(side), mCPU(isCPU) {}
		Player(const Player&) = default;

		void setCPU(bool isCPU = true) { mCPU = isCPU; }

		Colour getSide() const { return mSide; }

		bool operator== (const Player& other)
		{
			return mSide == other.mSide;
		}
		bool operator== (const Colour& other)
		{
			return mSide == other;
		}

		bool operator!= (const Player& other)
		{
			return !(*this == other);
		}
		bool operator!= (const Colour& other)
		{
			return !(*this == other);
		}

	private:
		Colour mSide = Colour::None;
		bool mCPU = false;
	};

}