#include "SandboxLayer.h"

namespace Laby {

	static constexpr glm::vec<2, i32> TileSize = glm::vec<2, i32>{ 64 };
	static constexpr glm::vec2 TileSizeF = glm::vec2{ 64.0f };

	void SandboxLayer::OnAttach()
	{
		FramebufferSpec fbSpec;
		fbSpec.width = 16 * TileSize.x;
		fbSpec.height = 16 * TileSize.y;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Ref<Framebuffer>::Create(fbSpec);
		mCamera = OrthographicCamera(0.0f, StaticCast<f32>(fbSpec.width), 0.0f, StaticCast<f32>(fbSpec.height));
	}

	void SandboxLayer::OnDetach()
	{
	}

	void SandboxLayer::OnUpdate(Timestep ts)
	{
		mFramebuffer->Bind();

		Renderer::SetClearColor({ 1, 0, 1, 1 });
		Renderer::Clear();

		const auto& spec = mFramebuffer->GetSpecification();

		Renderer2D::BeginState();
		//for (f32 y = 0; y < spec.height; y += 16.0f)
		//{
		//	for (f32 x = 0; x < spec.width; x += 16.0f)
		//	{
		//		glm::vec4 colour = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
		//		Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, colour);
		//	}
		//}
		Renderer2D::EndState();

		mFramebuffer->Unbind();
	}

	void SandboxLayer::OnRender()
	{
		Renderer2D::ResetStats();

		Widgets::BeginDockspace();
		ImGui::Begin("Demo");

		static bool demo = true; //Enable demo
		ImGui::ShowDemoWindow(&demo);

		ImGui::Image((ImTextureID)(uintptr_t)mFramebuffer->GetTextureID(), ImVec2{ 1280, 720 });

		ImGui::End();
		Widgets::EndDockspace();
	}

	void SandboxLayer::OnEvent(Event& e)
	{
		e.Dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(OnKeyPressedEvent));
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