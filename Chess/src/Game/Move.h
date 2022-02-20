#pragma once

#include <Labyrinth.h>
#include "GameComponents.h"
#include "Player.h"

namespace Labyrinth {

	class Move
	{
	public:
		Move() = default;
		Move(BoardState& boardState, Entity& piece, Entity& src, Entity& dest, bool checked, const std::vector<Entity>& attackingPieces);
		~Move();
				
		bool isValidMove() const { return mValidMove; }

		void resolve(Player& currTurn);

	private:
		bool ExecuteMove();
		void UndoMove();

		bool WillCauseCheck(const std::vector<Entity>& attackingPieces);

	private:
		BoardState* mBoardState = nullptr;

		Entity* mPiece = nullptr;
		PieceComponent* mPieceComp = nullptr;

		Entity* mSource = nullptr;
		SquareComponent* mSourceSquare = nullptr;

		Entity* mTarget = nullptr;
		SquareComponent* mTargetSquare = nullptr;

		Entity mCaptured;

		bool mPieceFirstMove = false;
		bool mValidMove = false;
	};

}