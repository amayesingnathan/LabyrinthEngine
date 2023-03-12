#include "SandboxLayer.h"

using imcpp::Widgets;

namespace Laby {

	static constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
	static constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };

	void SandboxLayer::onAttach()
	{
		FramebufferSpec fbSpec;
		fbSpec.width = 16 * TileSize.x;
		fbSpec.height = 16 * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Ref<Framebuffer>::Create(fbSpec);
		mCamera = OrthographicCamera(0.0f, StaticCast<f32>(fbSpec.width), 0.0f, StaticCast<f32>(fbSpec.height));
	}

	void SandboxLayer::onDetach()
	{
	}

	void SandboxLayer::onUpdate(Timestep ts)
	{
		Renderer::Submit([this]()
		{
			mFramebuffer->bind();

			Renderer::SetClearColor({ 1, 0, 1, 1 });
			Renderer::Clear();

			Renderer2D::BeginState();
			for (i64 y = 0; y < 16; y++)
			{
				for (i64 x = 0; x < 16; x++)
				{
					glm::vec2 pos = { x * TileSize.x, y * TileSize.y };
					pos += 0.5f * TileSizeF;

					Renderer2D::DrawRotatedQuad(pos, TileSizeF, 0, mSquareColours[rand() % 4]);
				}
			}
			Renderer2D::EndState();

			mFramebuffer->unbind();
		});
	}

	void SandboxLayer::onImGuiRender()
	{
		Renderer2D::ResetStats();

		Widgets::BeginDockspace();
		ImGui::Begin("Demo");

		static bool demo = true; //Enable demo
		ImGui::ShowDemoWindow(&demo);

		ImGui::Image((ImTextureID)(uintptr_t)mFramebuffer->getTextureID(), ImVec2{ 1280, 720 });

		ImGui::End();
		Widgets::EndDockspace();
	}

	void SandboxLayer::onEvent(Event& e)
	{
		e.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(OnKeyPressedEvent));
	}

	bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		switch (e.keyCode)
		{
		case Key::Escape:
		{
			Application::Close();
			return true;
		}
		}

		return false;
	}
}