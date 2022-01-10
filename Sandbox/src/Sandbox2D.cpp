#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), mCameraController(1280.0f / 720.0f, true)
{}

void Sandbox2D::onAttach()
{
	LAB_PROFILE_FUNCTION();

	mCheckerboardTexture = Labyrinth::Texture2D::Create("assets/textures/checkerboard.png");
}

void Sandbox2D::onDetach()
{
	LAB_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Labyrinth::Timestep ts)
{
	LAB_PROFILE_FUNCTION();

	mCameraController.onUpdate(ts);
	
	Labyrinth::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Labyrinth::RenderCommand::Clear();
		
	{
		LAB_PROFILE_SCOPE("Renderer Prep");
		Labyrinth::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Labyrinth::RenderCommand::Clear();
	}

	{
		LAB_PROFILE_SCOPE("Renderer Draw");
		Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());
		Labyrinth::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), mRotatedSquareColor);
		Labyrinth::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, mSquareColor);
		Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f}, { 1.8f, 1.8f }, mCheckerboardTexture);
		Labyrinth::Renderer2D::EndState();
	}
}

void Sandbox2D::onImGuiRender()
{
	LAB_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Rotated Square Color", glm::value_ptr(mRotatedSquareColor));
	ImGui::ColorEdit4("Square Color", glm::value_ptr(mSquareColor));
	ImGui::End();
}

void Sandbox2D::onEvent(Labyrinth::Event& e)
{
	mCameraController.onEvent(e);
}
