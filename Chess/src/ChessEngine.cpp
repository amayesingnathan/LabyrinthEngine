#include "ChessEngine.h"

#pragma warning(disable:26451)

namespace Labyrinth {

	void Chess::GetValidMoves(BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();
		const BoardPosition& startPos = pieceComp.position;
		outMoves.clear();

		switch (pieceComp.type)
		{
		case PieceType::Pawn:
		{
			outMoves.reserve(4);
			GetValidPawnMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		case PieceType::Rook:
		{
			outMoves.reserve(14);
			GetValidLineMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		case PieceType::Knight:
		{
			outMoves.reserve(8);
			GetValidKnightMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		case PieceType::Bishop:
		{
			outMoves.reserve(13);
			GetValidDiagMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		case PieceType::Queen:
		{
			outMoves.reserve(28);

			GetValidLineMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);
			GetValidDiagMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		case PieceType::King:
		{
			outMoves.reserve(8);
			GetValidKingMoves(startPos, boardState, piece, outMoves, oppPieces, check4check);

			break;
		}
		default: break;
		}
	}

	bool Chess::WillMoveCauseCheck(Move& move, std::vector<Entity>& oppPieces)
	{
		bool causesCheck = false;

		ExecuteMove(move);

		for (auto& piece : oppPieces)
		{
			if (piece == move.capturedPiece) continue;

 			auto& pieceComp = piece.getComponent<PieceComponent>();
			if (pieceComp.type == PieceType::Pawn) continue;

			std::vector<Move> checkCheck;
			GetValidMoves(*(move.boardState), piece, checkCheck, oppPieces, false);

			if (checkCheck.empty()) continue;

			for (const auto& checkMove : checkCheck)
			{
				if (!checkMove.targetSquare->currentPiece) continue;

				const auto& pieceInSquare = checkMove.targetSquare->currentPiece.getComponent<PieceComponent>();
				if (pieceInSquare.type == PieceType::King && pieceInSquare.colour != pieceComp.colour)
				{
					causesCheck = true;
					break;
				}
			}

			if (causesCheck)
				break;
		}

		UndoMove(move);

		return causesCheck;
	}

	void Chess::ExecuteMove(Move& move)
	{
		auto& trans = move.piece->getComponent<TransformComponent>().translation;

		const auto& squareTrans = move.target->getComponent<TransformComponent>().translation;
		trans = { squareTrans.x, squareTrans.y, trans.z };
		move.pieceComp->position = move.targetSquare->position;

		if (move.targetSquare->currentPiece)
			move.capturedPiece = move.targetSquare->currentPiece;

		move.sourceSquare->currentPiece = {};
		move.targetSquare->currentPiece = *(move.piece);

		if (move.pieceComp->unmoved)
		{
			move.pieceComp->unmoved = false;
			move.pieceFirstMove = true;
		}
	}

	void Chess::ResetPiece(Move& move)
	{
		auto& trans = move.piece->getComponent<TransformComponent>().translation;
		const auto& squareTrans = move.source->getComponent<TransformComponent>().translation;
		trans = { squareTrans.x, squareTrans.y, trans.z };
	}

	void Chess::UndoMove(Move& move)
	{
		//Reset piece transform
		auto& trans = move.piece->getComponent<TransformComponent>().translation;
		const auto& squareTrans = move.source->getComponent<TransformComponent>().translation;
		trans.x = squareTrans.x;
		trans.y = squareTrans.y;

		// Reset board position
		move.pieceComp->position = move.sourceSquare->position;
		move.sourceSquare->currentPiece = *(move.piece);

		// Restore piece if one was captured
		if (move.capturedPiece)
			move.targetSquare->currentPiece = move.capturedPiece;
		else
			move.targetSquare->currentPiece = {};

		if (move.pieceFirstMove) // Reset unmoved flag if this piece was previously unmoved
			move.pieceComp->unmoved = true;

		move.capturedPiece = {};
	}

	void Chess::GetValidPawnMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();
		int moveDir = (pieceComp.colour == Colour::White) ? 1 : -1;

		if ((startPos.y + moveDir) >= 0 && (startPos.y + moveDir) < 8)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y + moveDir).getComponent<SquareComponent>().currentPiece;
			if (!pieceInTarget)
			{
				Move testMove(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y + moveDir), oppPieces);
				if (!check4check || !WillMoveCauseCheck(testMove, oppPieces))
					outMoves.push_back(std::move(testMove));
			}
		}

		if ((startPos.y + moveDir) >= 0 && (startPos.y + moveDir) < 8)
		{
			if (startPos.x != 0)
			{
				const Entity& leftCapture = boardState(startPos.x - 1, startPos.y + moveDir);
				if (leftCapture.getComponent<SquareComponent>().currentPiece)
				{
					Move testMove(boardState, piece, boardState(startPos), boardState(startPos.x - 1, startPos.y + moveDir), oppPieces);
					if (!check4check || !WillMoveCauseCheck(testMove, oppPieces))
						outMoves.push_back(std::move(testMove));
				}
			}
			if (startPos.x != 7)
			{
				const Entity& rightCapture = boardState(startPos.x + 1,startPos.y + moveDir);
				if (rightCapture.getComponent<SquareComponent>().currentPiece)
				{
					Move testMove(boardState, piece, boardState(startPos), boardState(startPos.x + 1, startPos.y + moveDir), oppPieces);
					if (!check4check || !WillMoveCauseCheck(testMove, oppPieces))
						outMoves.push_back(std::move(testMove));
				}
			}
		}

		if (pieceComp.unmoved)
		{
			const Entity& pieceInTarget = boardState(startPos.x, startPos.y + 2 * moveDir).getComponent<SquareComponent>().currentPiece;
			if (!pieceInTarget)
			{
				Move testMove(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y + (2 * moveDir)), oppPieces);
				if (!check4check ||!WillMoveCauseCheck(testMove, oppPieces))
					outMoves.push_back(std::move(testMove));
			}
		}
	}

	void Chess::GetValidLineMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();

		int xOffset = 1;
		int yOffset = 1;

		bool blockedLeft = false;
		bool blockedRight = false;
		bool blockedDown = false;
		bool blockedUp = false;

		while ((startPos.x - xOffset >= 0) || (startPos.x + xOffset < 8) || (startPos.y - yOffset >= 0) || (startPos.y + yOffset < 8))
		{
			if ((startPos.x - xOffset >= 0) && !blockedLeft)
			{
				const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					blockedLeft = true;

				Move left(boardState, piece, boardState(startPos), boardState(startPos.x - xOffset, startPos.y), oppPieces);
				if (!check4check || !WillMoveCauseCheck(left, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(left));
					}
					else
						outMoves.push_back(std::move(left));
				}
			}
			if ((startPos.x + xOffset) < 8 && !blockedRight)
			{
				const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					blockedRight = true;

				Move right(boardState, piece, boardState(startPos), boardState(startPos.x + xOffset, startPos.y), oppPieces);
				if (!check4check || !WillMoveCauseCheck(right, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(right));
					}
					else
						outMoves.push_back(std::move(right));
				}
			}
			xOffset++;

			if ((startPos.y - yOffset) >= 0 && !blockedDown)
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					blockedDown = true;

				Move down(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y - yOffset), oppPieces);
				if (!check4check || !WillMoveCauseCheck(down, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(down));
					}
					else
						outMoves.push_back(std::move(down));
				}
			}
			if ((startPos.y + yOffset) < 8 && !blockedUp)
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					blockedUp = true;

				Move up(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y + yOffset), oppPieces);
				if (!check4check || !WillMoveCauseCheck(up, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(up));
					}
					else
						outMoves.push_back(std::move(up));
				}
			}
			yOffset++;

		}
	}

	void Chess::GetValidDiagMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();

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
					nnBlocked = true;

				Move downLeft(boardState, piece, boardState(startPos), boardState(startPos.x - i, startPos.y - i), oppPieces);
				if (!check4check || !WillMoveCauseCheck(downLeft, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(downLeft));
					}
					else
						outMoves.push_back(std::move(downLeft));
				}
			}
			if ((startPos.x - i) >= 0 && (startPos.y + i) < 8 && !npBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x - i, startPos.y + i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					npBlocked = true;
				
				Move upLeft(boardState, piece, boardState(startPos), boardState(startPos.x - i, startPos.y + i), oppPieces);
				if (!check4check || !WillMoveCauseCheck(upLeft, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(upLeft));
					}
					else
						outMoves.push_back(std::move(upLeft));
				}
			}
			if ((startPos.x + i < 8) && (startPos.y - i >= 0) && !pnBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x + i, startPos.y - i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					pnBlocked = true;

				Move downRight(boardState, piece, boardState(startPos), boardState(startPos.x + i, startPos.y - i), oppPieces);
				if (!check4check || !WillMoveCauseCheck(downRight, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(downRight));
					}
					else
						outMoves.push_back(std::move(downRight));
				}
			}
			if ((startPos.x + i) < 8 && (startPos.y + i) < 8 && !ppBlocked)
			{
				const Entity& pieceInTarget = boardState(startPos.x + i, startPos.y + i).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
					ppBlocked = true;

				Move upRight(boardState, piece, boardState(startPos), boardState(startPos.x + i, startPos.y + i), oppPieces);
				if (!check4check || !WillMoveCauseCheck(upRight, oppPieces))
				{
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(upRight));
					}
					else
						outMoves.push_back(std::move(upRight));
				}
			}
		}
	}

	void Chess::GetValidKnightMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();

		if (startPos.x < 6)
		{
			if (startPos.y < 7)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x + 2, startPos.y + 1), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x + 2, startPos.y + 1).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
			if (startPos.y > 0)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x + 2, startPos.y - 1), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x + 2, startPos.y - 1).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
		}
		if (startPos.x > 1)
		{
			if (startPos.y < 7)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x - 2, startPos.y + 1), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x - 2, startPos.y + 1).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
			if (startPos.y > 0)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x - 2, startPos.y - 1), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x - 2, startPos.y - 1).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
		}
		if (startPos.x < 7)
		{
			if (startPos.y < 6)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x + 1, startPos.y + 2), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x + 1, startPos.y + 2).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
			if (startPos.y > 1)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x + 1, startPos.y - 2), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x + 1, startPos.y - 2).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
		}
		if (startPos.x > 0)
		{
			if (startPos.y < 6)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x - 1, startPos.y + 2), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x - 1, startPos.y + 2).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
			if (startPos.y > 1)
			{
				Move knightMove(boardState, piece, boardState(startPos), boardState(startPos.x - 1, startPos.y - 2), oppPieces);
				if (!check4check || !WillMoveCauseCheck(knightMove, oppPieces))
				{
					const Entity& pieceInTarget = boardState(startPos.x - 1, startPos.y - 2).getComponent<SquareComponent>().currentPiece;
					if (pieceInTarget)
					{
						if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
							outMoves.push_back(std::move(knightMove));
					}
					else
						outMoves.push_back(std::move(knightMove));
				}
			}
		}
	}

	void Chess::GetValidKingMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check)
	{
		const auto& pieceComp = piece.getComponent<PieceComponent>();

		int xOffset = 1;
		int yOffset = 1;

		if ((startPos.x - xOffset) >= 0)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x - xOffset, startPos.y), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
		if ((startPos.x + xOffset) < 8)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x + xOffset, startPos.y), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}

		if ((startPos.y - yOffset) >= 0)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y - yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
		if ((startPos.y + yOffset) < 8)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x, startPos.y + yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}

		if ((startPos.x - xOffset) >= 0 && (startPos.y - yOffset) >= 0)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x - xOffset, startPos.y - yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
		if ((startPos.x - xOffset) >= 0 && (startPos.y + yOffset) < 8)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x - xOffset, startPos.y + yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x - xOffset, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
		if ((startPos.x + xOffset) < 8 && (startPos.y - yOffset) >= 0)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x + xOffset, startPos.y - yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y - yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
		if ((startPos.x + xOffset) < 8 && (startPos.y + yOffset) < 8)
		{
			Move kingMove(boardState, piece, boardState(startPos), boardState(startPos.x + xOffset, startPos.y + yOffset), oppPieces);
			if (!check4check || !WillMoveCauseCheck(kingMove, oppPieces))
			{
				const Entity& pieceInTarget = boardState(startPos.x + xOffset, startPos.y + yOffset).getComponent<SquareComponent>().currentPiece;
				if (pieceInTarget)
				{
					if (pieceInTarget.getComponent<PieceComponent>().colour != pieceComp.colour)
						outMoves.push_back(std::move(kingMove));
				}
				else
					outMoves.push_back(std::move(kingMove));
			}
		}
	}

}