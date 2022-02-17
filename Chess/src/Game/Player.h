#pragma once

#include "GameComponents.h"

namespace Labyrinth {

	class Player
	{
	public:
		Player() = default;
		Player(Colour side, bool isCPU = false)
			: mSide(side), mCPU(isCPU) {}

		void setCPU(bool isCPU = true) { mCPU = isCPU; }

		Colour getSide() const { return mSide; }

	private:
		Colour mSide = Colour::None;
		bool mCPU = false;
	};

}