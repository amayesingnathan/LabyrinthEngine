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
		: Layer("ChessLayer"), mCameraController(1280.0f / 720.0f)
	{
	}

	void ChessLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportBounds[0] = { 0, 0 };
		mViewportBounds[1] = { fbSpec.width, fbSpec.height };

		mCurrentScene = CreateRef<Scene>();

		mBoard.create(mCurrentScene);
	}

	void ChessLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void ChessLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		mViewportFocused = Input::IsWindowFocused();
		mViewportHovered = Input::IsWindowHovered();

		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mCameraController.onResize(mViewportSize.x, mViewportSize.y);

			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		mCameraController.onUpdate(ts);

		Renderer2D::ResetStats();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		mFramebuffer->clearAttachment(1, -1);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= mViewportBounds[0].x;
		my -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = mFramebuffer->readPixel(1, mouseX, mouseY);
			mHoveredEntity = (pixelData == -1) ? Entity() : Entity((entt::entity)pixelData, mCurrentScene);
		}

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
		ImGui::Text("Hovered Entity: %d", mHoveredEntity.getEntID());
		ImGui::Text("Held Entity: %d", mSelectedEntity.getEntID());

		ImGui::End();
		Labyrinth::Renderer2D::ResetStats();
	}

	void ChessLayer::onEvent(Event& e)
	{
		mCameraController.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseMoved));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(ChessLayer::OnMouseButtonReleased));
	}

	bool ChessLayer::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(LAB_MOUSE_BUTTON_LEFT) && mSelectedEntity)
		{
			glm::vec2 delta = e.getPos() - mLastMousePos;
			auto& trans = mSelectedEntity.getComponent<TransformComponent>();
			trans.translation += glm::vec3(delta, 0);
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
				if (mHoveredEntity)
					mSelectedEntity = mHoveredEntity;
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
			if (mSelectedEntity)
				mSelectedEntity = {};
		}
		}

		return false;
	}

}