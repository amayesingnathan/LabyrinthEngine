#include "Move.h"

#include "../ChessEngine.h"

namespace Labyrinth {

	Move::Move(Entity& piece, Entity& src, Entity& dest)
		: mPiece(&piece), mPieceComp(&piece.getComponent<PieceComponent>()), 
		mSource(&src), mSourceSquare(&src.getComponent<SquareComponent>()),
		mTarget(&dest), mTargetSquare((dest) ? &dest.getComponent<SquareComponent>() : nullptr)
	{
		if (!dest)
		{
			mValidMove = false;
			return;
		}

		std::vector<BoardPosition> validMoves;
		Chess::GetValidMoves(*mPieceComp, validMoves);

		if (validMoves.empty())
			mValidMove = false;

		if (std::find(validMoves.begin(), validMoves.end(), mTargetSquare->position) == validMoves.end())
			mValidMove = false;
		else
			mValidMove = true;
	}

	void Move::resolve()
	{
		auto& trans = mPiece->getComponent<TransformComponent>().translation;

		if (mValidMove)
		{
			const auto& squareTrans = mTarget->getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };
			mPieceComp->position = mTargetSquare->position;

			if (mTargetSquare->currentPiece)
				mTargetSquare->currentPiece.destroy();

			mSourceSquare->currentPiece = {};
			mTargetSquare->currentPiece = *mPiece;

			if (mPieceComp->unmoved) mPieceComp->unmoved = false;
		}
		else
		{
			const auto& squareTrans = mSource->getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };
		}
	}

}