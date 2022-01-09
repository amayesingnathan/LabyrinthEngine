#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), mCameraController(1280.0f / 720.0f, true)
{}

void Sandbox2D::onAttach()
{
	mCheckerboardTexture = Labyrinth::Texture2D::Create("assets/textures/checkboard.png");
}

void Sandbox2D::onDetach()
{
}

void Sandbox2D::onUpdate(Labyrinth::Timestep ts)
{
	mCameraController.onUpdate(ts);

	Labyrinth::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Labyrinth::RenderCommand::Clear();

	Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());

	Labyrinth::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Labyrinth::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Labyrinth::Renderer2D::DrawQuad({ 0.2f, 0.5f}, { 0.5f, 0.5f }, mCheckerboardTexture);

	Labyrinth::Renderer2D::EndState();
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(mSquareColor));
	ImGui::End();
}

void Sandbox2D::onEvent(Labyrinth::Event& e)
{
	mCameraController.onEvent(e);
}
