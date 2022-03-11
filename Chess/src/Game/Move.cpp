#include "Move.h"

#include "Board.h"
#include "../ChessEngine.h"

namespace Labyrinth {

	Move::Move(BoardState& _boardState, Entity& _piece, Entity& src, Entity& dest, std::vector<Entity>& _oppPieces)
		: boardState(&_boardState), oppPieces(&_oppPieces),
		piece(&_piece), pieceComp(&_piece.getComponent<PieceComponent>()),
		source(&src), sourceSquare(&src.getComponent<SquareComponent>()),
		target(&dest), targetSquare((dest) ? &dest.getComponent<SquareComponent>() : nullptr)
	{
		if (!targetSquare)
			valid = false;
	}

	Move::~Move()
	{
		if (capturedPiece)
			capturedPiece.destroy();
	}

	MoveResolveFlags Move::resolve(Player& currTurn)
	{
		if (!valid) 
		{
			Chess::ResetPiece(*this);
			return MoveResolveFlags_Fail;
		}

		std::vector<Move> validMoves;
		Chess::GetValidMoves(*boardState, *piece, validMoves, *oppPieces);

		if (validMoves.empty())
		{
			Chess::ResetPiece(*this);
			return MoveResolveFlags_Fail;
		}

		if (std::find_if(validMoves.begin(), validMoves.end(), [&](Move& move)
			{
				return move.targetSquare == targetSquare;
			}) == validMoves.end())
		{
			Chess::ResetPiece(*this);
			return MoveResolveFlags_Fail;
		}

		if (currTurn != pieceComp->colour)
		{
			Chess::ResetPiece(*this);
			return MoveResolveFlags_Fail;
		}

		Chess::ExecuteMove(*this);

		if (currTurn == Colour::White)
			currTurn = Colour::Black;
		else
			currTurn = Colour::White;

		if (capturedPiece)
		{
			auto it = std::find_if(oppPieces->begin(), oppPieces->end(), [&](const Entity& piece)
				{
					return capturedPiece.getEntID() == piece.getEntID();
				});
			if (it != oppPieces->end())
				oppPieces->erase(it);

			capturedPiece.destroy();
			capturedPiece = {};
		}

		return MoveResolveFlags_Success;
	}

}