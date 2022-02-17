#pragma once

#include <Labyrinth.h>
#include "GameComponents.h"

namespace Labyrinth {

	class Move
	{
	public:
		Move() = default;
		Move(Entity& piece, Entity& src, Entity& dest);
				
		bool isValidMove() const { return mValidMove; }

		void resolve();

	private:
		Player mPlayer = {};

		Entity* mPiece = nullptr;
		PieceComponent* mPieceComp = nullptr;

		Entity* mSource = nullptr;
		SquareComponent* mSourceSquare = nullptr;

		Entity* mTarget = nullptr;
		SquareComponent* mTargetSquare = nullptr;

		bool mValidMove = false;
	};

}