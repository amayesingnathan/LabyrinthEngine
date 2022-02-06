#pragma once

#include <Labyrinth.h>

#include "GameEntities/Board.h"
#include "GameEntities/GameComponents.h"

namespace Labyrinth {

	class ChessLayer : public Layer
	{
	public:
		ChessLayer();
		virtual ~ChessLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Labyrinth::Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Labyrinth::Event & e) override;

	private:
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		Ref<Scene> mCurrentScene;

		Ref<Framebuffer> mFramebuffer;

		Board mBoard;

		float mBoardWidth;
		float mBoardHeight;
		glm::vec2 mBoardSquareSize;

		Entity mSelectedEntity;
		Entity mHoveredEntity;
		glm::vec2 mLastMousePos = { 0.0f, 0.0f };

		bool mViewportFocused =  false;
		bool mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};
	};
}