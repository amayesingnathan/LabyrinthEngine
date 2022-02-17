#pragma once

#include <Labyrinth.h>
#include "GameComponents.h"
#include "Player.h"

namespace Labyrinth {

	class Move
	{
	public:
		Move() = default;
		Move(const class Board& boardState, Entity& piece, Entity& src, Entity& dest);
				
		bool isValidMove() const { return mValidMove; }

		void resolve(Player& currTurn);

	private:
		Entity* mPiece = nullptr;
		PieceComponent* mPieceComp = nullptr;

		Entity* mSource = nullptr;
		SquareComponent* mSourceSquare = nullptr;

		Entity* mTarget = nullptr;
		SquareComponent* mTargetSquare = nullptr;

		bool mValidMove = false;
	};

}