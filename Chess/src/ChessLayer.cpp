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
		: Layer("ChessLayer"), 
		mCurrentScene(CreateRef<Scene>()), mViewportSize(Application::Get().getWindow().getSize()), 
		mBoard(mCurrentScene, mViewportSize)
	{
		mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
	}

	void ChessLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();


		FramebufferSpec fbSpec;
		fbSpec.width = Cast<uint32_t>(mViewportSize.x);
		fbSpec.height = Cast<uint32_t>(mViewportSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportBounds[0] = { 0, 0 };
		mViewportBounds[1] = { fbSpec.width, fbSpec.height };


	}

	void ChessLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void ChessLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		mBoard.onUpdate();

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

		mBoard.onImGuiRender();

		ImGui::End();
		Labyrinth::Renderer2D::ResetStats();
	}

	void ChessLayer::onEvent(Event& e)
	{
		mBoard.onEvent(e);
	}

}