#pragma once

#include "Labyrinth.h"

#include "Game/GameComponents.h"
#include "Game/Move.h"

namespace Labyrinth {

	class Chess
	{
	public:
		static void GetValidMoves(BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check = true);

		static void ExecuteMove(Move& move);
		static void ResetPiece(Move& move);
		static void UndoMove(Move& move);
		static bool WillMoveCauseCheck(Move& move, std::vector<Entity>& oppPieces);

		static bool CausedCheck(Move& move, std::vector<Entity>& oppPieces);
		static bool CausedCheckmate(Move& move, std::vector<Entity>& playerPieces, std::vector<Entity>& oppPieces);

	private:
		static void GetValidPawnMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check);
		static void GetValidLineMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check);
		static void GetValidDiagMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check);
		static void GetValidKnightMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check);
		static void GetValidKingMoves(const BoardPosition startPos, BoardState& boardState, Entity& piece, std::vector<Move>& outMoves, std::vector<Entity>& oppPieces, bool check4check);

	};
}