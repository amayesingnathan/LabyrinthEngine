#pragma once

#include "Labyrinth.h"

#include "Game/GameComponents.h"

namespace Labyrinth {

	class Chess
	{
	public:
		static void GetValidMoves(const BoardState& boardState, const Entity& piece, std::vector<BoardPosition>& outMoves, bool checked = false);
		static void GetValidMoves(const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked = false);

	private:
		static void GetValidPawnMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked);
		static void GetValidLineMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked);
		static void GetValidDiagMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked);
		static void GetValidKnightMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked);
		static void GetValidKingMoves(const BoardPosition startPos, const BoardState& boardState, const PieceComponent& piece, std::vector<BoardPosition>& outMoves, bool checked);
	};
}