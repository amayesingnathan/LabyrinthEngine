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
			: Layer("Example"), mCamera(-1.6f, 1.6f, -0.9f, 0.9f), mCameraPosition(0.0f), mCameraMatRot(glm::mat3(1.0f)), mTrianglePos(0.0f)
		{
			mLastMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());

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
			glm::vec3 moveVec(0.0f);

			//Calculate how much to move camera from mouse being dragged
			if (mDragging)
			{
				float moveX = mLastMousePos.x - Input::GetMouseX(); 
				float moveY = Input::GetMouseY() - mLastMousePos.y;
				moveVec += (mCameraMoveSpeed / (16 * mCameraZoom)) * ts * (mCameraMatRot * glm::vec3(moveX, moveY, 0.0f));
			}

			if (Input::IsKeyPressed(LAB_KEY_LSHIFT)) mCameraMatRot = glm::mat3(1.0f); //Lock movement to world axis if holding shift

			//Move the camera depending on it's current rotation for more intuitive movement.
			//Similarly scale camera move speed by zoom factor.
			if (Input::IsKeyPressed(LAB_KEY_A))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(-1.0f, 0.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_D))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(1.0f, 0.0f, 0.0f));

			if (Input::IsKeyPressed(LAB_KEY_W))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(0.0f, 1.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_S))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(0.0f, -1.0f, 0.0f));
			mCameraPosition += moveVec; moveVec = glm::vec3(0.0f);

			//Move square
			if (Input::IsKeyPressed(LAB_KEY_J))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(-1.0f, 0.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_L))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(1.0f, 0.0f, 0.0f));

			if (Input::IsKeyPressed(LAB_KEY_I))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(0.0f, 1.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_K))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (mCameraMatRot * glm::vec3(0.0f, -1.0f, 0.0f));
			mTrianglePos += moveVec; moveVec = glm::vec3(0.0f);

			if (Input::IsKeyPressed(LAB_KEY_Q))
				mCameraRotation += mCameraRotationSpeed * ts;
			if (Input::IsKeyPressed(LAB_KEY_E))
				mCameraRotation -= mCameraRotationSpeed * ts;

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			mCamera.setPosition(mCameraPosition);
			mCamera.setRotation(mCameraRotation);
			mCamera.setZoom(mCameraZoom);
			mCameraMatRot = mCamera.getRotationMat();
			mLastMousePos = { Input::GetMouseX(), Input::GetMouseY() };

			Renderer::BeginState(mCamera);
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
			if (event.getEventType() == Labyrinth::EventType::KeyPressed)
			{
				Labyrinth::KeyPressedEvent& e = (Labyrinth::KeyPressedEvent&)event;
				//KEY PRESS EVENTS HANDLING HERE

			}
			if (event.getEventType() == Labyrinth::EventType::MouseScrolled)
			{
				Labyrinth::MouseScrolledEvent& e = (Labyrinth::MouseScrolledEvent&)event;
				float zoomDelta = e.getYOffset() / 25.0f;
				mCameraZoom = (mCameraZoom + zoomDelta > 0.1f) ? mCameraZoom + zoomDelta : 0.1f; //Maximum zoom out of 10x
			}
			if (event.getEventType() == Labyrinth::EventType::MouseButtonPressed)
			{
				Labyrinth::MouseButtonPressedEvent& e = (Labyrinth::MouseButtonPressedEvent&)event;
				if (e.getMouseButton() == LAB_MOUSE_BUTTON_LEFT)
					mDragging = true;
			}
			if (event.getEventType() == Labyrinth::EventType::MouseButtonReleased)
			{
				Labyrinth::MouseButtonReleasedEvent& e = (Labyrinth::MouseButtonReleasedEvent&)event;
				if (e.getMouseButton() == LAB_MOUSE_BUTTON_LEFT)
					mDragging = false;
			}

			//if (event.getEventType() == Labyrinth::EventType::MouseMoved)
			//{
			//	Labyrinth::MouseMovedEvent& e = (Labyrinth::MouseMovedEvent&)event;
			//	mLastMousePos = { e.getX(), e.getY() };

			//}
		}

	private:
		Ref<Shader> mFlatColorShader, mTextureShader;
		Ref<VertexArray> mSquareVA;

		Ref<Texture2D> mTexture;

		OrthographicCamera mCamera;
		glm::vec3 mCameraPosition;
		float mCameraMoveSpeed = 2.5f;
		float mCameraZoom = 1.0f;
		glm::mat3 mCameraMatRot;
		float mCameraRotation = 0.0f;
		float mCameraRotationSpeed = 90.0f;

		glm::vec3 mTrianglePos;
		float mTriangleMoveSpeed = 1.0f;

		glm::vec3 mSquareColor = { 0.2f, 0.3f, 0.8f };

		//Event Flags
		bool mDragging = false;
		glm::vec2 mLastMousePos = { 0.0f, 0.0f };
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