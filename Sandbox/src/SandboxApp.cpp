#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "Platforms/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

namespace Labyrinth {

	class ExampleLayer : public Labyrinth::Layer
	{
	public:
		ExampleLayer()
			: Layer("Example"), mCameraController(1280.0f / 720.0f, true)
		{
		}

		virtual void onImGuiRender() override
		{
			ImGui::Begin("Settings");
			ImGui::ColorEdit3("Square Color", glm::value_ptr(mSquareColor));
			ImGui::End();
		}

		void onUpdate(Timestep ts) override
		{
			mCameraController.onUpdate(ts);

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			Renderer::BeginState(mCameraController.getCamera());

			Renderer::EndState();
		}

		void onEvent(Labyrinth::Event& event) override
		{
			if (event.getEventType() == Labyrinth::EventType::KeyPressed)
			{
				Labyrinth::KeyPressedEvent& e = (Labyrinth::KeyPressedEvent&)event;
				//KEY PRESS EVENTS HANDLING HERE

			}
		}

	private:
		ShaderLibrary mShaderLibrary;
		Ref<VertexArray> mSquareVA;

		Ref<Texture2D> mTexture;

		OrthographicCameraController mCameraController;

		glm::vec3 mSquareColor = { 0.2f, 0.3f, 0.8f };
	};

}

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{
		//pushLayer(new Labyrinth::ExampleLayer());
		pushLayer(new Sandbox2D());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}