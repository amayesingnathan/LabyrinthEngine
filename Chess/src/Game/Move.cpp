#include "Move.h"

#include "Board.h"
#include "../ChessEngine.h"

namespace Labyrinth {

	Move::Move(BoardState& boardState, Entity& piece, Entity& src, Entity& dest, bool checked, const std::vector<Entity>& attackingPieces)
		: mBoardState(&boardState),
		mPiece(&piece), mPieceComp(&piece.getComponent<PieceComponent>()), 
		mSource(&src), mSourceSquare(&src.getComponent<SquareComponent>()),
		mTarget(&dest), mTargetSquare((dest) ? &dest.getComponent<SquareComponent>() : nullptr)
	{
		if (!mTargetSquare)
		{
			mValidMove = false;
			return;
		}

		std::vector<BoardPosition> validMoves;
		Chess::GetValidMoves(boardState, *mPieceComp, validMoves, checked);

		if (validMoves.empty())
			mValidMove = false; 

		if (std::find(validMoves.begin(), validMoves.end(), mTargetSquare->position) == validMoves.end())
			mValidMove = false;
		else
			mValidMove = true;

		if (WillCauseCheck(attackingPieces))
			mValidMove = false;
	}

	Move::~Move()
	{
		if (mCaptured)
			mCaptured.destroy();
	}

	void Move::resolve(Player& currTurn)
	{
		if (currTurn != mPieceComp->colour) mValidMove = false;

		if (!ExecuteMove()) return;

		if (currTurn == Colour::White)
			currTurn = Colour::Black;
		else
			currTurn = Colour::White;

		if (mCaptured)
		{
			mCaptured.destroy();
			mCaptured = {};
		}
	}

	bool Move::ExecuteMove()
	{
		bool success = false;
		auto& trans = mPiece->getComponent<TransformComponent>().translation;

		if (mValidMove)
		{
			const auto& squareTrans = mTarget->getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };
			mPieceComp->position = mTargetSquare->position;

			if (mTargetSquare->currentPiece)
				mCaptured = mTargetSquare->currentPiece;

			mSourceSquare->currentPiece = {};
			mTargetSquare->currentPiece = *mPiece;

			if (mPieceComp->unmoved)
			{
				mPieceComp->unmoved = false;
				mPieceFirstMove = true;
			}
			
			success = true;
		}
		else
		{
			const auto& squareTrans = mSource->getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };

			success = false;
		}

		return success;
	}

	void Move::UndoMove()
	{
		auto& trans = mPiece->getComponent<TransformComponent>().translation;
		const auto& squareTrans = mSource->getComponent<TransformComponent>().translation;

		mPieceComp->position = mSourceSquare->position;
		trans = { squareTrans.x, squareTrans.y, trans.z };

		mSourceSquare->currentPiece = *mPiece;
		if (mCaptured)
			mTargetSquare->currentPiece = mCaptured;
		else
			mTargetSquare->currentPiece = {};

		if (mPieceFirstMove)
			mPieceComp->unmoved = true;

		mCaptured = {};

	}

	bool Move::WillCauseCheck(const std::vector<Entity>& attackingPieces)
	{
		bool causesCheck = false;

		ExecuteMove();

		for (const auto& piece : attackingPieces)
		{
			const auto& pieceComp = piece.getComponent<PieceComponent>();
			if (pieceComp.type == PieceType::Pawn) continue;

			std::vector<BoardPosition> checkCheck;
			Chess::GetValidMoves(*mBoardState, pieceComp, checkCheck);

			if (checkCheck.empty()) continue;

			for (const auto& move : checkCheck)
			{
				const auto& targetSquare = (*mBoardState)(move.x, move.y).getComponent<SquareComponent>();
				if (!targetSquare.currentPiece) continue;

				const auto& pieceInSquare = targetSquare.currentPiece.getComponent<PieceComponent>();
				if (pieceInSquare.type == PieceType::King && pieceInSquare.colour != pieceComp.colour)
				{
					causesCheck = true;
					break;
				}
			}

			if (causesCheck)
				break;
		}

		UndoMove();

		return causesCheck;
	}

}