#include <Labyrinth.h>

#include "Platforms/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	class ExampleLayer : public Labyrinth::Layer
	{
	public:
		ExampleLayer()
			: Layer("Example"), mCameraController(1280.0f / 720.0f)
		{
			mSquareVA.reset(VertexArray::Create());

			float squareVertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
			};

			Ref<VertexBuffer> squareVB;
			squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
			squareVB->setLayout({
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float2, "aTexCoord" }
				});
			mSquareVA->addVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			Ref<IndexBuffer> squareIB;
			squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
			mSquareVA->setIndexBuffer(squareIB);

			std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;

			out vec3 vPosition;

			uniform mat4 uViewProjection;
			uniform mat4 uTransform;

			void main()	
			{
				vPosition = aPosition;
				gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
			}
		)";

			std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			uniform vec3 uColor;

			void main()
			{
				color = vec4(uColor, 1.0);
			}
		)";

			mFlatColorShader.reset(Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

			std::string texShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec2 aTexCoord;

			out vec2 vTexCoord;

			uniform mat4 uViewProjection;
			uniform mat4 uTransform;

			void main()	
			{
				vTexCoord = aTexCoord;
				gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
			}
		)";

			std::string texShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec2 vTexCoord;

			uniform sampler2D uTexture;

			void main()
			{
				color = texture(uTexture, vTexCoord);
			}
		)";

			mTextureShader.reset(Shader::Create(texShaderVertexSrc, texShaderFragmentSrc));

			mTexture = Texture2D::Create("assets/textures/checkerboard.png");

			std::dynamic_pointer_cast<OpenGLShader>(mTextureShader)->bind();
			std::dynamic_pointer_cast<OpenGLShader>(mTextureShader)->uploadUniformInt("uTexture", 0);
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
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			std::dynamic_pointer_cast<OpenGLShader>(mFlatColorShader)->bind();
			std::dynamic_pointer_cast<OpenGLShader>(mFlatColorShader)->uploadUniformFloat3("uColor", mSquareColor);

			for (int y = 0; y < 20; y++)
			{
				for (int x = 0; x < 20; x++)
				{
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					Renderer::Send(mFlatColorShader, mSquareVA, transform);
				}
			}

			mTexture->bind();
			Renderer::Send(mTextureShader, mSquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
			//glm::mat4 transform = glm::translate(glm::mat4(1.0f), mTrianglePos);

			//Triangle
			//Renderer::Send(mShader, mVertexArray, transform);

			Renderer::EndState();
		}

		void onEvent(Labyrinth::Event& event) override
		{
			mCameraController.onEvent(event);
			if (event.getEventType() == Labyrinth::EventType::KeyPressed)
			{
				Labyrinth::KeyPressedEvent& e = (Labyrinth::KeyPressedEvent&)event;
				//KEY PRESS EVENTS HANDLING HERE

			}
		}

	private:
		Ref<Shader> mFlatColorShader, mTextureShader;
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
		pushLayer(new Labyrinth::ExampleLayer());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}