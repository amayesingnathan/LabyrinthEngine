#pragma once

#include <Labyrinth.h>

#include "GameComponents.h"

namespace Labyrinth {

	class Board 
	{
	public:
		Board() = default;

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void create(Ref<Scene> inScene, const glm::vec2& viewportSize);

		void onViewportResize(const glm::vec2& newSize);

	private:
		void ResetPieces();

		bool LoadBoard(const std::string& filepath);

		void BuildWhitePieces();
		void BuildBlackPieces();
		void BuildRow(int row, std::vector<Entity>& squares);

		void ResolveMove();
		bool IsValidSquare(const PieceComponent& piece, const SquareComponent& square);

		void DrawFramebuffers();

	private:
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		Ref<Scene> mContext = nullptr;

		Ref<Framebuffer> mPiecesFramebuffer;
		Ref<Framebuffer> mBoardFramebuffer;

		Entity mSelectedPiece;
		glm::vec3 mLastPiecePos;
		Entity mHoveredPiece;
		Entity mHoveredSquare;

		Entity mBoard;
		Entity mWhite;
		Entity mBlack;

		bool mViewportFocused = false;
		bool mViewportHovered = false;

		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mLastMousePos = { 0.0f, 0.0f };

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