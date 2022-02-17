#include "ChessLayer.h"

#include "Game/GameComponents.h"

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
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportBounds[0] = { 0, 0 };
		mViewportBounds[1] = { fbSpec.width, fbSpec.height };

		mCurrentScene = CreateRef<Scene>();
		mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);

		mBoard.create(mCurrentScene, mViewportSize);
	}

	void ChessLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void ChessLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		mBoard.onUpdate(ts);

		mViewportFocused = Input::IsWindowFocused();
		mViewportHovered = Input::IsWindowHovered();

		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mBoard.onViewportResize(mViewportSize);
			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		Renderer2D::ResetStats();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

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
		//ImGui::Text("Hovered Piece: %d", mHoveredPiece.getEntID());
		//ImGui::Text("Held Piece: %d", mSelectedPiece.getEntID());
		//ImGui::Text("Hovered Square: %d", mHoveredSquare.getEntID());

		ImGui::End();
		Labyrinth::Renderer2D::ResetStats();
	}

	void ChessLayer::onEvent(Event& e)
	{
		mBoard.onEvent(e);
	}

}