#pragma once

#include "glm/glm.hpp"

#include "Labyrinth/Scene/Entity.h"

namespace Labyrinth {

	enum class Colour
	{
		None = -1,
		White,
		Black,
		End
	};

	enum class PieceType
	{
		None = -1,
		Pawn,
		Rook,
		Knight,
		Bishop,
		Queen,
		King,
		End
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

	class BoardState
	{
	public:
		BoardState() = default;

		Entity& operator()(size_t row, size_t column)
		{
			return mBoard[(column * 8) + row];
		}

		const Entity& operator()(size_t row, size_t column) const
		{
			return mBoard[(column * 8) + row];
		}

		Entity& operator()(const BoardPosition& pos)
		{
			return mBoard[(pos.y * 8) + pos.x];
		}

		const Entity& operator()(const BoardPosition& pos) const
		{
			return mBoard[(pos.y * 8) + pos.x];
		}


	private:
		std::array<Entity, 64> mBoard;
	};

	struct BoardComponent
	{
		BoardState boardState;

		BoardComponent() = default;
	};

	struct SquareComponent
	{
		Colour colour = Colour::None;
		BoardPosition position;
		Entity currentPiece;

		SquareComponent() = default;
		SquareComponent(Colour c, const BoardPosition& pos, Entity piece = {})
			: colour(c), position(pos), currentPiece(piece) {}
		SquareComponent(const SquareComponent&) = default;

		bool operator== (const SquareComponent& other)
		{
			return position == other.position;
		}

	};

	struct PieceComponent
	{
		Colour colour = Colour::None;
		PieceType type = PieceType::None;
		BoardPosition position;
		bool unmoved = true;
		bool active = true;

		PieceComponent() = default;
		PieceComponent(Colour c, PieceType t, const BoardPosition& pos)
			: colour(c), type(t), position(pos) {}
		PieceComponent(const PieceComponent&) = default;


	};

}