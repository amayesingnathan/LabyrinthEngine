#include "ChessLayer.h"

#include "GameEntities/GameComponents.h"

#include "Labyrinth/Maths/Maths.h"

#include <entt.hpp>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	ChessLayer::ChessLayer()
		: Layer("ChessLayer")
	{
	}

	void ChessLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		mViewportSize = Application::Get().getWindow().getSize();

		FramebufferSpec fbSpec;
		fbSpec.width = Cast<uint32_t>(mViewportSize.x);
		fbSpec.height = Cast<uint32_t>(mViewportSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);
		mBoardFramebuffer = Framebuffer::Create(fbSpec);
		mPiecesFramebuffer = Framebuffer::Create(fbSpec);

		mViewportBounds[0] = { 0, 0 };
		mViewportBounds[1] = { fbSpec.width, fbSpec.height };

		mCurrentScene = CreateRef<Scene>();
		mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);

		mBoard.create(mCurrentScene);
	}

	void ChessLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void ChessLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		auto mousePos = Input::GetMousePosition();
		mousePos.x -= mViewportBounds[0].x;
		mousePos.y -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		mousePos.y = viewportSize.y - mousePos.y;
		int mouseX = (int)mousePos.x;
		int mouseY = (int)mousePos.y;

		mViewportFocused = Input::IsWindowFocused();
		mViewportHovered = Input::IsWindowHovered();

		if (FramebufferSpec spec = mBoardFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mBoardFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mPiecesFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		Renderer2D::ResetStats();

		DrawFramebuffers();
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			// Check for hovered board square
			mBoardFramebuffer->bind();
			int boardPixelData = mBoardFramebuffer->readPixel(1, mouseX, mouseY);
			mHoveredSquare = (boardPixelData == -1) ? Entity() : Entity((entt::entity)boardPixelData, mCurrentScene);

			// Check for hovered piece
			mPiecesFramebuffer->bind();
			int piecePixelData = mPiecesFramebuffer->readPixel(1, mouseX, mouseY);
			mHoveredPiece = (piecePixelData == -1) ? Entity() : Entity((entt::entity)piecePixelData, mCurrentScene);
		}

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		mFramebuffer->clearAttachment(1, -1);

		mCurrentScene->onUpdateRuntime(ts);

		Renderer2D::DrawFramebuffer(mFramebuffer);
	}

	void ChessLayer::onImGuiRender()
	{
		Application::Get().getImGuiLayer()->blockEvents(!mViewportFocused && !mViewportHovered);

		ImGui::Begin("Stats");

		auto stats = Labyrinth::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());
		ImGui::Text("Hovered Piece: %d", mHoveredPiece.getEntID());
		ImGui::Text("Held Piece: %d", mSelectedPiece.getEntID());
		ImGui::Text("Hovered Square: %d", mHoveredSquare.getEntID());

		ImGui::End();
		Labyrinth::Renderer2D::ResetStats();
	}

	void ChessLayer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseMoved));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseButtonReleased));
	}

	void ChessLayer::DrawFramebuffers()
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = mCurrentScene->view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [trans, cam] = view.get<TransformComponent, CameraComponent>(entity);

				if (cam.primary)
				{
					mainCamera = &cam.camera;
					cameraTransform = trans;
				}
			}
		}

		if (mainCamera)
		{
			mBoardFramebuffer->bind();
			Renderer2D::BeginState(*mainCamera, cameraTransform);

			RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			mBoardFramebuffer->clearAttachment(1, -1);

			auto boardView = mCurrentScene->view<TransformComponent, SpriteRendererComponent, SquareComponent>();
			for (auto entity : boardView)
			{
				auto& [transform, sprite] = boardView.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}

			Renderer2D::EndState();

			mPiecesFramebuffer->bind();
			Renderer2D::BeginState(*mainCamera, cameraTransform);

			RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			mPiecesFramebuffer->clearAttachment(1, -1);

			auto piecesView = mCurrentScene->view<TransformComponent, SpriteRendererComponent, PieceComponent>();
			for (auto entity : piecesView)
			{
				auto& [transform, sprite] = piecesView.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}

			Renderer2D::EndState();
		}

	}

	bool ChessLayer::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(LAB_MOUSE_BUTTON_LEFT) && mSelectedPiece)
		{
			glm::vec2 delta = e.getPos() - mLastMousePos;
			auto& trans = mSelectedPiece.getComponent<TransformComponent>();
			trans.translation += (0.01f * glm::vec3(delta.x, -delta.y, 0));
		}

		mLastMousePos = e.getPos();
		return false;
	}

	bool ChessLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.getMouseButton())
		{
		case LAB_MOUSE_BUTTON_LEFT:
		{
			if (mViewportHovered && !Input::IsKeyPressed(LAB_KEY_LALT))
				if (mHoveredPiece)
				{
					mSelectedPiece = mHoveredPiece;
				}
		}
		break;
		}

		return false;
	}

	bool ChessLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		switch (e.getMouseButton())
		{
		case LAB_MOUSE_BUTTON_LEFT:
		{
			if (mSelectedPiece)
				mSelectedPiece = {};
		}
		}

		return false;
	}

}