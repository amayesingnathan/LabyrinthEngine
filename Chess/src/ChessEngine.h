#pragma once

#include "Labyrinth.h"

#include "GameEntities/GameComponents.h"

namespace Labyrinth {

	namespace Chess
	{
		static void GetValidMoves(const PieceComponent& piece, std::vector<BoardPosition>& outMoves)
		{
			const BoardPosition& startPos = piece.position;
			outMoves.clear();

			switch (piece.type)
			{
			case PieceType::Pawn:
			{
				outMoves.reserve(2);
				
				int moveDir = (piece.colour == Colour::White) ? 1 : -1;
				outMoves.emplace_back(startPos.x, startPos.y + moveDir);

				if (piece.unmoved)
				{
					outMoves.emplace_back(startPos.x, startPos.y + (2 * moveDir));
				}
				break;
			}
			case PieceType::Rook:
			{
				outMoves.reserve(14);

				int xOffset = 1;
				int yOffset = 1;

				while ((startPos.x - xOffset >= 0) || (startPos.x + xOffset < 8) || (startPos.y - yOffset >= 0) || (startPos.y + yOffset < 8))
				{
					if (startPos.x - xOffset >= 0)
						outMoves.emplace_back(startPos.x - xOffset, startPos.y);
					if (startPos.x + xOffset < 8)
						outMoves.emplace_back(startPos.x + xOffset, startPos.y);
					xOffset++;

					if (startPos.y - yOffset >= 0)
						outMoves.emplace_back(startPos.x, startPos.y - yOffset);
					if (startPos.y + yOffset < 8)
						outMoves.emplace_back(startPos.x, startPos.y + yOffset);
					yOffset++;

				}
				break;
			}
			case PieceType::Knight:
			{
				outMoves.reserve(8);
				
				if (startPos.x < 6)
				{
					if (startPos.y < 7)
						outMoves.emplace_back(startPos.x + 2, startPos.y + 1);
					if (startPos.y > 0)
						outMoves.emplace_back(startPos.x + 2, startPos.y - 1);
				}
				if (startPos.x > 1)
				{
					if (startPos.y < 7)
						outMoves.emplace_back(startPos.x - 2, startPos.y + 1);
					if (startPos.y > 0)
						outMoves.emplace_back(startPos.x - 2, startPos.y - 1);
				}
				if (startPos.x < 7)
				{
					if (startPos.y < 6)
						outMoves.emplace_back(startPos.x + 1, startPos.y + 2);
					if (startPos.y > 1)
						outMoves.emplace_back(startPos.x + 1, startPos.y - 2);
				}
				if (startPos.x > 0)
				{
					if (startPos.y < 6)
						outMoves.emplace_back(startPos.x - 1, startPos.y + 2);
					if (startPos.y > 1)
						outMoves.emplace_back(startPos.x - 1, startPos.y - 2);
				}

				break;
			}
			case PieceType::Bishop:
			{
				outMoves.reserve(13);

				for (int i = 1; i <= 4; i++)
				{
					if (startPos.x - i >= 0 && startPos.y - i >= 0)
						outMoves.emplace_back(startPos.x - i, startPos.y - i);
					if (startPos.x - i >= 0 && startPos.y + i < 8)
						outMoves.emplace_back(startPos.x - i, startPos.y + i);
					if (startPos.x + i < 8 && startPos.y - i >= 0)
						outMoves.emplace_back(startPos.x + i, startPos.y - i);
					if (startPos.x + i < 8 && startPos.y + i < 8)
						outMoves.emplace_back(startPos.x + i, startPos.y + i);
				}
				break;
			}
			case PieceType::Queen:
			{
				outMoves.reserve(28);

				int xOffset = 1;
				int yOffset = 1;

				while ((startPos.x - xOffset >= 0) || (startPos.x + xOffset < 8) || (startPos.y - yOffset >= 0) || (startPos.y + yOffset < 8))
				{
					if (startPos.x - xOffset >= 0)
						outMoves.emplace_back(startPos.x - xOffset, startPos.y);
					if (startPos.x + xOffset < 8)
						outMoves.emplace_back(startPos.x + xOffset, startPos.y);
					xOffset++;

					if (startPos.y - yOffset >= 0)
						outMoves.emplace_back(startPos.x, startPos.y - yOffset);
					if (startPos.y + yOffset < 8)
						outMoves.emplace_back(startPos.x, startPos.y + yOffset);
					yOffset++;
				}

				for (int i = 1; i <= 4; i++)
				{
					if (startPos.x - i >= 0 && startPos.y - i >= 0)
						outMoves.emplace_back(startPos.x - i, startPos.y - i);
					if (startPos.x - i >= 0 && startPos.y + i < 8)
						outMoves.emplace_back(startPos.x - i, startPos.y + i);
					if (startPos.x + i < 8 && startPos.y - i >= 0)
						outMoves.emplace_back(startPos.x + i, startPos.y - i);
					if (startPos.x + i < 8 && startPos.y + i < 8)
						outMoves.emplace_back(startPos.x + i, startPos.y + i);
				}
				break;
			}
			case PieceType::King:
			{
				outMoves.reserve(8);

				int xOffset = 1;
				int yOffset = 1;

				if (startPos.x - xOffset >= 0)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y);
				if (startPos.x + xOffset < 8)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y);

				if (startPos.y - yOffset >= 0)
					outMoves.emplace_back(startPos.x, startPos.y - yOffset);
				if (startPos.y + yOffset < 8)
					outMoves.emplace_back(startPos.x, startPos.y + yOffset);
				
				if (startPos.x - xOffset >= 0 && startPos.y - yOffset >= 0)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y - yOffset);
				if (startPos.x - xOffset >= 0 && startPos.y + yOffset < 8)
					outMoves.emplace_back(startPos.x - xOffset, startPos.y + yOffset);
				if (startPos.x + xOffset < 8 && startPos.y - yOffset >= 0)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y - yOffset);
				if (startPos.x + xOffset < 8 && startPos.y + yOffset < 8)
					outMoves.emplace_back(startPos.x + xOffset, startPos.y + yOffset);

				break;
			}
			default: break;
			}
		}
	}
}