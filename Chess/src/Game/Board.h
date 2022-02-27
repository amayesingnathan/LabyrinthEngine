#pragma once

#include <Labyrinth.h>

#include "GameComponents.h"
#include "Player.h"
#include "Move.h"

namespace Labyrinth {

	class Board 
	{
	public:
		Board(Ref<Scene> inScene, const glm::vec2& viewportSize);

		void onUpdate();
		void onEvent(Event& e);
		void onImGuiRender();

		void onViewportResize(const glm::vec2& newSize);

	private:
		void ResetPieces();

		bool LoadBoard(const std::string& filepath);

		void BuildWhitePieces();
		void BuildBlackPieces();
		void BuildRow(int row, std::vector<Entity>& squares);

		void ResolveMove();

		void DrawBoardFramebuffer();

	private:
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		Ref<Scene> mContext = nullptr;

		Ref<Framebuffer> mBoardFramebuffer = nullptr;

		Entity mSelectedPiece = {};
		Entity mLastSquare = {};

		std::vector<Move> mPieceMoves;

		Entity mHoveredSquare = {};

		const glm::vec4 mWhiteColour = { 0.800000012f, 0.529411793f, 0.125490203f, 1.0f };
		const glm::vec4 mBlackColour = { 0.250980407f, 0.0313725509f, 0.0352941193f, 1.0f };
		const glm::vec4 mValidMoveColour = { 0.211f, 0.988f, 0.937f, 1.0f };

		const std::unordered_map<PieceType, int> mPieceScores = {	
			{PieceType::Pawn, 1},
			{PieceType::Rook, 5},
			{PieceType::Bishop, 3},
			{PieceType::Knight, 3},
			{PieceType::Queen, 9} };

		Entity mBoard = {};
		BoardState* mBoardState = nullptr;

		Entity mCamera;

		Entity mWhitePiecesRoot = {};
		std::vector<Entity> mWhitePieces;
		Entity mWhiteKing = {};

		Entity mBlackPiecesRoot = {};
		std::vector<Entity> mBlackPieces;
		Entity mBlackKing = {};

		Player mCurrPlayer = Colour::White;
		Move mNextMove = {};
		bool mWhiteChecked = false;
		bool mBlackChecked = false;

		bool mViewportFocused = false;
		bool mViewportHovered = false;

		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mLastMousePos = { 0.0f, 0.0f };

	};

}