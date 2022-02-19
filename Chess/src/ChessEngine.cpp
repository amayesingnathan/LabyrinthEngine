#include "ChessEngine.h"

namespace Labyrinth {

	void Chess::GetValidMoves(const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		const BoardPosition& startPos = piece.position;
		outMoves.clear();

		switch (piece.type)
		{
		case PieceType::Pawn:
		{
			outMoves.reserve(4);
			GetValidPawnMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		case PieceType::Rook:
		{
			outMoves.reserve(14);
			GetValidLineMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		case PieceType::Knight:
		{
			outMoves.reserve(8);
			GetValidKnightMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		case PieceType::Bishop:
		{
			outMoves.reserve(13);
			GetValidDiagMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		case PieceType::Queen:
		{
			outMoves.reserve(28);

			GetValidLineMoves(startPos, boardState, piece, outMoves, checked);
			GetValidDiagMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		case PieceType::King:
		{
			outMoves.reserve(8);
			GetValidKingMoves(startPos, boardState, piece, outMoves, checked);

			break;
		}
		default: break;
		}
	}

	void Chess::GetValidMoves(const BoardState& boardState, const Entity& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		GetValidMoves(boardState, piece.getComponent<PieceComponent>(), outMoves, checked);
	}

	void Chess::GetValidPawnMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		int moveDir = (piece.colour == Colour::White) ? 1 : -1;

		if ((startPos.y + moveDir) >= 0 && (startPos.y + moveDir) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y + moveDir).getComponent<SquareComponent>().currentPiece;
			if (!pieceInTarget)
				outMoves.emplace_back(startPos.x, startPos.y + moveDir);
		}

		if ((startPos.y + moveDir) >= 0 && (startPos.y + moveDir) < 8)
		{
			if (startPos.x != 0)
			{
				const Entity& leftCapture = boardState(startPos.x - 1, startPos.y + moveDir);
				if (leftCapture.getComponent<SquareComponent>().currentPiece)
					outMoves.emplace_back(startPos.x - 1, startPos.y + moveDir);
			}
			if (startPos.x != 7)
			{
				const Entity& rightCapture = boardState(startPos.x + 1,startPos.y + moveDir);
				if (rightCapture.getComponent<SquareComponent>().currentPiece)
					outMoves.emplace_back(startPos.x + 1, startPos.y + moveDir);
			}
		}

		if (piece.unmoved)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y + 2 * moveDir).getComponent<SquareComponent>().currentPiece;
			if (!pieceInTarget)
				outMoves.emplace_back(startPos.x, startPos.y + (2 * moveDir));
		}
	}

	void Chess::GetValidLineMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{

		int xOffset = 1;
		int yOffset = 1;

		bool xBlockedLeft = false;
		bool xBlockedRight = false;
		bool yBlockedLeft = false;
		bool yBlockedRight = false;

		while ((startPos.x - xOffset >= 0) || (startPos.x + xOffset < 8) || (startPos.y - yOffset >= 0) || (startPos.y + yOffset < 8))
		{
			if ((startPos.x - xOffset >= 0) && !xBlockedLeft)
			{
				const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					xBlockedLeft = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - xOffset, startPos.y);
				}
				else
					outMoves.emplace_back(startPos.x - xOffset, startPos.y);
			}
			if ((startPos.x + xOffset) < 8 && !xBlockedRight)
			{
				const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					xBlockedRight = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + xOffset, startPos.y);
				}
				else
					outMoves.emplace_back(startPos.x + xOffset, startPos.y);
			}
			xOffset++;

			if ((startPos.y - yOffset) >= 0 && !yBlockedLeft)
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					yBlockedLeft = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x, startPos.y - yOffset);
				}
				else
					outMoves.emplace_back(startPos.x, startPos.y - yOffset);
			}
			if ((startPos.y + yOffset) < 8 && !yBlockedRight)
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					yBlockedRight = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x, startPos.y + yOffset);
				}
				else
					outMoves.emplace_back(startPos.x, startPos.y + yOffset);
			}
			yOffset++;

		}
	}

	void Chess::GetValidDiagMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		bool pnBlocked = false;
		bool ppBlocked = false;
		bool npBlocked = false;
		bool nnBlocked = false;

		for (int i = 1; i <= 4; i++)
		{
			if ((startPos.x - i) >= 0 && (startPos.y - i) >= 0 && !nnBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x - i, startPos.y - i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					nnBlocked = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - i, startPos.y - i);
				}
				else
					outMoves.emplace_back(startPos.x - i, startPos.y - i);
			}
			if ((startPos.x - i) >= 0 && (startPos.y + i) < 8 && !npBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x - i, startPos.y + i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					npBlocked = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - i, startPos.y + i);
				}
				else
					outMoves.emplace_back(startPos.x - i, startPos.y + i);
			}
			if ((startPos.x + i < 8) && (startPos.y - i >= 0) && !pnBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x + i, startPos.y - i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					pnBlocked = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + i, startPos.y - i);
				}
				else
					outMoves.emplace_back(startPos.x + i, startPos.y - i);
			}
			if ((startPos.x + i) < 8 && (startPos.y + i) < 8 && !ppBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x + i, startPos.y + i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					ppBlocked = true;
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + i, startPos.y + i);
				}
				else
					outMoves.emplace_back(startPos.x + i, startPos.y + i);
			}
		}
	}

	void Chess::GetValidKnightMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		if (startPos.x < 6)
		{
			if (startPos.y < 7)
			{
				const Entity& pieceInTarget = boardState(startPos.x + 2, startPos.y + 1).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + 2, startPos.y + 1);
				}
				else
					outMoves.emplace_back(startPos.x + 2, startPos.y + 1);
			}
			if (startPos.y > 0)
			{
				const Entity& pieceInTarget = boardState(startPos.x + 2, startPos.y - 1).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + 2, startPos.y - 1);
				}
				else
					outMoves.emplace_back(startPos.x + 2, startPos.y - 1);
			}
		}
		if (startPos.x > 1)
		{
			if (startPos.y < 7)
			{
				const Entity& pieceInTarget = boardState(startPos.x - 2, startPos.y + 1).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - 2, startPos.y + 1);
				}
				else
					outMoves.emplace_back(startPos.x - 2, startPos.y + 1);
			}
			if (startPos.y > 0)
			{
				const Entity& pieceInTarget = boardState(startPos.x - 2, startPos.y - 1).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - 2, startPos.y - 1);
				}
				else
					outMoves.emplace_back(startPos.x - 2, startPos.y - 1);
			}
		}
		if (startPos.x < 7)
		{
			if (startPos.y < 6)
			{
				const Entity& pieceInTarget = boardState(startPos.x + 1, startPos.y + 2).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + 1, startPos.y + 2);
				}
				else
					outMoves.emplace_back(startPos.x + 1, startPos.y + 2);
			}
			if (startPos.y > 1)
			{
				const Entity& pieceInTarget = boardState(startPos.x + 1, startPos.y - 2).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x + 1, startPos.y - 2);
				}
				else
					outMoves.emplace_back(startPos.x + 1, startPos.y - 2);
			}
		}
		if (startPos.x > 0)
		{
			if (startPos.y < 6)
			{
				const Entity& pieceInTarget = boardState(startPos.x - 1, startPos.y + 2).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - 1, startPos.y + 2);
				}
				else
					outMoves.emplace_back(startPos.x - 1, startPos.y + 2);
			}
			if (startPos.y > 1)
			{
				const Entity& pieceInTarget = boardState(startPos.x - 1, startPos.y - 2).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
						outMoves.emplace_back(startPos.x - 1, startPos.y - 2);
				}
				else
					outMoves.emplace_back(startPos.x - 1, startPos.y - 2);
			}
		}
	}

	void Chess::GetValidKingMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked)
	{
		int xOffset = 1;
		int yOffset = 1;

		if ((startPos.x - xOffset) >= 0)
		{
			const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y);
			}
			else
				outMoves.emplace_back(startPos.x - xOffset, startPos.y);
		}
		if ((startPos.x + xOffset) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y);
			}
			else
				outMoves.emplace_back(startPos.x + xOffset, startPos.y);
		}

		if ((startPos.y - yOffset) >= 0)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x, startPos.y - yOffset);
			}
			else
				outMoves.emplace_back(startPos.x, startPos.y - yOffset);
		}
		if ((startPos.y + yOffset) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x, startPos.y + yOffset);
			}
			else
				outMoves.emplace_back(startPos.x, startPos.y + yOffset);
		}

		if ((startPos.x - xOffset) >= 0 && (startPos.y - yOffset) >= 0)
		{
			const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y - yOffset);
			}
			else
				outMoves.emplace_back(startPos.x - xOffset, startPos.y - yOffset);
		}
		if ((startPos.x - xOffset) >= 0 && (startPos.y + yOffset) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y + yOffset);
			}
			else
				outMoves.emplace_back(startPos.x - xOffset, startPos.y + yOffset);
		}
		if ((startPos.x + xOffset) < 8 && (startPos.y - yOffset) >= 0)
		{
			const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y - yOffset);
			}
			else
				outMoves.emplace_back(startPos.x + xOffset, startPos.y - yOffset);
		}
		if ((startPos.x + xOffset) < 8 && (startPos.y + yOffset) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
			if (pieceInTarget)
			{
				if (pieceInTarget.getComponent<PieceComponent>().colour != piece.colour)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y + yOffset);
			}
			else
				outMoves.emplace_back(startPos.x + xOffset, startPos.y + yOffset);
		}
	}

}