#pragma once

#include <Labyrinth.h>

#include "GameComponents.h"
#include "Player.h"
#include "Move.h"

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

		void DrawFramebuffers();

	private:
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		Ref<Scene> mContext = nullptr;

		Ref<Framebuffer> mBoardFramebuffer = nullptr;

		Entity mSelectedPiece = {};
		Entity mLastSquare = {};

		Entity mHoveredSquare = {};

		Entity mBoard = {};
		Entity mWhitePieces = {};
		Entity mBlackPieces = {};

		Player mCurrPlayer = Colour::White;
		Move nextMove = {};

		bool mViewportFocused = false;
		bool mViewportHovered = false;

		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mLastMousePos = { 0.0f, 0.0f };

	};

}