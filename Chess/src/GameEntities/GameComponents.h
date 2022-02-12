#pragma once

#include "glm/glm.hpp"

namespace Labyrinth {

	enum class Colour
	{
		None = -1,
		White,
		Black
	};

	enum class PieceType
	{
		None = -1,
		Pawn,
		Rook,
		Knight,
		Bishop,
		Queen,
		King
	};

	struct BoardPosition
	{
		int x = 0;
		int y = 0;

		BoardPosition() = default;
		BoardPosition(int x, int y)
			: x(x), y(y) {}
		BoardPosition(const glm::vec2& pos)
			: x(Cast<int>(pos.x)), y(Cast<int>(pos.y)) {}

		BoardPosition(const BoardPosition&) = default;
		
		glm::vec2 getVecPosition() const { return glm::vec2(x, y); }

		bool operator== (const BoardPosition& other)
		{
			return ((x == other.x) && (y == other.y));
		}
	};

	struct SquareComponent
	{
		Colour colour = Colour::None;
		BoardPosition position;
		class Entity* currentPiece = nullptr;

		SquareComponent() = default;
		SquareComponent(Colour c, const BoardPosition& pos, class Entity* piece = nullptr)
			: colour(c), position(pos), currentPiece(piece) {}
		SquareComponent(const SquareComponent&) = default;

	};

	struct PieceComponent
	{
		Colour colour = Colour::None;
		PieceType type = PieceType::None;
		BoardPosition position;
		bool active = true;

		PieceComponent() = default;
		PieceComponent(Colour c, PieceType t, const BoardPosition& pos)
			: colour(c), type(t), position(pos), active(true) {}
		PieceComponent(const PieceComponent&) = default;

		std::string getString()
		{
			std::string subtexKey;

			switch (colour)
			{
			case Colour::White:	subtexKey = "White"; break;
			case Colour::Black:	subtexKey = "Black"; break;
			}
			enum Pieces { Pawn = 0, King, Queen, Rook, Knight, Bishop };

			switch (type)
			{
			case PieceType::Pawn:	subtexKey += "Pawn"; break;
			case PieceType::King:	subtexKey += "King"; break;
			case PieceType::Queen:	subtexKey += "Queen"; break;
			case PieceType::Rook:	subtexKey += "Rook"; break;
			case PieceType::Knight:	subtexKey += "Knight"; break;
			case PieceType::Bishop:	subtexKey += "Bishop"; break;
			}

			return subtexKey;
		}

	};

}