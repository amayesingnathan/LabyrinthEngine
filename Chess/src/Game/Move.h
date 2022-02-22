#pragma once

#include <Labyrinth.h>
#include "GameComponents.h"
#include "Player.h"

namespace Labyrinth {

	struct Move
	{
		BoardState* boardState = nullptr;
		std::vector<Entity>* oppPieces = nullptr;

		Entity* piece = nullptr;
		PieceComponent* pieceComp = nullptr;

		Entity* source = nullptr;
		SquareComponent* sourceSquare = nullptr;

		Entity* target = nullptr;
		SquareComponent* targetSquare = nullptr;

		Entity capturedPiece;

		bool pieceFirstMove = false;
		bool valid = true;

		bool resolve(Player& currTurn);

		Move() = default;
		Move(BoardState& boardState, Entity& piece, Entity& src, Entity& dest, std::vector<Entity>& attackingPieces);
		~Move();
	};

}