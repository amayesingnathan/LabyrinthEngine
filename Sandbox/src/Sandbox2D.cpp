#include "Sandbox2D.h"
#include <Labyrinth.h>
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), mCameraController(1280.0f / 720.0f, true)
{}

void Sandbox2D::onAttach()
{
	LAB_PROFILE_FUNCTION();

	// mSpriteSheet = Labyrinth::Texture2DSheet::Create("assets/textures/minecraft.png", { 64.0f, 64.0f });
	// //mTNT = Labyrinth::SubTexture2D::CreateFromCoords(mSpriteSheet, { 8.0f, 0.0f }, { 1.0f, 1.0f });
	// mSpriteSheet->createSubTex("TNT", { 8.0f, 0.0f }, {1.0f, 1.0f});
}

void Sandbox2D::onDetach()
{
	LAB_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Labyrinth::Timestep ts)
{
	LAB_PROFILE_FUNCTION();

	mCameraController.onUpdate(ts);
			
	{
		LAB_PROFILE_SCOPE("Renderer Prep");
		Labyrinth::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Labyrinth::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50;

		LAB_PROFILE_SCOPE("Renderer Draw");
		//Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());
		
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 10.0f, 10.0f }, mSpriteSheet->getTex());
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 2.0f, 2.0f }, mSpriteSheet->getSubTex("TNT"));
		//Labyrinth::Renderer2D::DrawQuad({ 2.0f, 2.0f }, { 2.0f, 2.0f }, Labyrinth::SubTexture2D::CreateFromCoords(mSpriteSheet, { 13.0f, 3.0f }, { 1.0f, 1.0f }));
		//Labyrinth::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, -rotation, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.5f }, mSquareColor);
		//Labyrinth::Renderer2D::DrawQuad({ -0.5f, -0.5f }, { 0.25f, 0.25f }, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.75f, 0.75f }, { 0.5f, 0.5f }, mSquareColor);
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.1f, 0.1f }, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f}, { 1.8f, 1.8f }, mCheckerboardTexture);
		//Labyrinth::Renderer2D::DrawRotatedQuad({ 2.0f, 2.0f }, { 1.0f, 1.0f }, rotation, mCheckerboardTexture, 2.0f);
		//Labyrinth::Renderer2D::EndState();


		//Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());
		//for (f32 y = -5.0f; y < 5.0f; y += 0.5f)
		//{
		//	for (f32 x = -5.0f; x < 5.0f; x += 0.5f)
		//	{
		//		glm::vec4 colour = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
		//		Labyrinth::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, colour);
		//	}
		//}
		//Labyrinth::Renderer2D::EndState();

	}
}

void Sandbox2D::onImGuiRender()
{
	LAB_PROFILE_FUNCTION();

	Labyrinth::Renderer2D::ResetStats();

	ImGui::Begin("Settings");

	static bool demo = true; //Enable demo
	ImGui::ShowDemoWindow(&demo);

	auto stats = Labyrinth::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());


	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	ImGui::ColorEdit4("Square Color", glm::value_ptr(mSquareColor));
	ImGui::ColorEdit4("Square Color 2", glm::value_ptr(mSquareColor2));

	//uint32_t textureID = mCheckerboardTexture->getRendererID();
	//ImGui::Image((ImTextureID)(uintptr_t)textureID, ImVec2{ 1280, 720 });

	ImGui::End();
}

void Sandbox2D::onEvent(Labyrinth::Event& e)
{
	mCameraController.onEvent(e);
}
