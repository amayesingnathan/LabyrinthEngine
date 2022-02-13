#pragma once

#include "Labyrinth.h"

#include "GameEntities/GameComponents.h"

namespace Labyrinth {

	namespace Chess
	{
		static void GetValidMoves(const PieceComponent& piece, std::vector<BoardPosition>& validMoves)
		{
			const BoardPosition& startPos = piece.position;
			switch (piece.type)
			{
			case PieceType::Pawn:
			{
				int moveDir = (piece.colour == Colour::White) ? 1 : -1;
				validMoves.emplace_back(startPos.x, startPos.y + moveDir);

				if (piece.unmoved)
				{
					validMoves.emplace_back(startPos.x, startPos.y + (2 * moveDir));
				}
				break;
			}
			}
		}
	}
}