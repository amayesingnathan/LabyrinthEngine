#pragma once

#include <Labyrinth.h>

#include "Game/Board.h"
#include "Game/GameComponents.h"

namespace Labyrinth {

	class ChessLayer : public Layer
	{
	public:
		ChessLayer();
		virtual ~ChessLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event & e) override;

	private:

	private:
		Ref<Scene> mCurrentScene;

		Ref<Framebuffer> mFramebuffer;

		bool mViewportFocused = false;
		bool mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};

		Board mBoard;

		float mBoardWidth;
		float mBoardHeight;
		glm::vec2 mBoardSquareSize;
	};
}