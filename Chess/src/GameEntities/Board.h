#pragma once

#include <Labyrinth.h>

namespace Labyrinth {

	class Board 
	{
	public:
		Board() = default;

		void create(Ref<Scene> inScene);

	private:
		void ResetPieces();

		bool LoadBoard(const std::string& filepath);

	private:
		Ref<Scene> mContext = nullptr;

		Entity mBoard = {};
		Entity mWhite = {};
		Entity mBlack = {};

		const uint32_t MaxPawnCount = 8;
		const uint32_t MaxRookCount = 2;
		const uint32_t MaxKnightCount = 2;
		const uint32_t MaxBishopCount = 2;
		const uint32_t MaxQueenCount = 1;
		const uint32_t MaxKingCount = 1;

		uint32_t wPawnCount = 0;
		uint32_t wRookCount = 0;
		uint32_t wKnightCount = 0;
		uint32_t wBishopCount = 0;
		uint32_t wQueenCount = 0;
		uint32_t wKingCount = 0;

		uint32_t bPawnCount = 0;
		uint32_t bRookCount = 0;
		uint32_t bKnightCount = 0;
		uint32_t bBishopCount = 0;
		uint32_t bQueenCount = 0;
		uint32_t bKingCount = 0;
	};

}