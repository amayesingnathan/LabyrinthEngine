#include <Labyrinth.h>

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES

namespace Labyrinth {

	class ExampleLayer : public Labyrinth::Layer
	{
	public:
		ExampleLayer()
			: Layer("Example"), mCamera(-1.6f, 1.6f, -0.9f, 0.9f), mCameraPosition(0.0f), mTrianglePos(0.0f)
		{
			mVertexArray.reset(VertexArray::Create());

			float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
				 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
				 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
			};

			Ref<VertexBuffer> vertexBuffer;
			vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

			BufferLayout layout = {
				{ShaderDataType::Float3, "aPosition"},
				{ShaderDataType::Float4, "aColour"}
			};

			vertexBuffer->setLayout(layout);
			mVertexArray->addVertexBuffer(vertexBuffer);

			uint32_t indices[3] = { 0, 1, 2 };

			Ref<IndexBuffer> indexBuffer;
			indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			mVertexArray->setIndexBuffer(indexBuffer);

			mSquareVA.reset(VertexArray::Create());

			float squareVertices[3 * 4] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.5f,  0.5f, 0.0f,
				-0.5f,  0.5f, 0.0f
			};

			Ref<VertexBuffer> squareVB;
			squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
			squareVB->setLayout({
				{ ShaderDataType::Float3, "aPosition" }
				});
			mSquareVA->addVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			Ref<IndexBuffer> squareIB;
			squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
			mSquareVA->setIndexBuffer(squareIB);

			std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColour;

			uniform mat4 uViewProjection;
			uniform mat4 uTransform;

			out vec3 vPosition;
			out vec4 vColour;

			void main()
			{
				vPosition = aPosition;
				vColour = aColour;
				gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);		
			}
		)";

			std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;
			in vec4 vColour;

			void main()
			{
				color = vec4(vPosition * 0.5 + 0.5, 1.0);
				color = vColour;
			}
		)";

			mShader.reset(new Shader(vertexSrc, fragmentSrc));

			std::string blueShaderVertexSrc = R"(
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

			std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

			mBlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));

			//Second shapes
			mTriVA.reset(VertexArray::Create());

			float tridata[3 * 3] = {
				 2.5f,  2.5f,  0.0f, 
				 2.5f,  1.6f,  0.0f,
				 1.6f,  2.05f, 0.0f
			};

			Ref<VertexBuffer> newTriangleVBuffer;
			newTriangleVBuffer.reset(VertexBuffer::Create(tridata, sizeof(tridata)));
			newTriangleVBuffer->setLayout({
				{Labyrinth::ShaderDataType::Float3, "aPosition"}
			});
			mTriVA->addVertexBuffer(newTriangleVBuffer);

			uint32_t triind[3] = { 0, 1, 2 };

			Ref<IndexBuffer> newTriangleIBuffer;
			newTriangleIBuffer.reset(IndexBuffer::Create(triind, sizeof(triind) / sizeof(uint32_t)));
			mTriVA->setIndexBuffer(newTriangleIBuffer);

			std::string triVSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;

			uniform mat4 uViewProjection;
			uniform mat4 uTransform;

			out vec3 vPosition;

			void main()
			{
				vPosition = aPosition;
				gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
			}
		)";

			std::string triFSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			void main()
			{
				color = vec4(vPosition * 0.5 + 0.5, 1.0);
			}
		)";

			mTriShader.reset(new Shader(triVSrc, triFSrc));
		}

		virtual void onImGuiRender() override
		{
		}

		void onUpdate(Timestep ts) override
		{
			glm::vec3 moveVec(0.0f);
			glm::mat3 rotMat = mCamera.getRotationMat();
			if (Input::IsKeyPressed(LAB_KEY_LSHIFT)) rotMat = glm::mat3(1.0f);
			//Move the camera depending on it's current rotation for more intuitive movement.
			//Similarly scale camera move speed by zoom factor.
			if (Input::IsKeyPressed(LAB_KEY_A))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(-1.0f, 0.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_D))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(1.0f, 0.0f, 0.0f));

			if (Input::IsKeyPressed(LAB_KEY_W))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(0.0f, 1.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_S))
				moveVec += (mCameraMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(0.0f, -1.0f, 0.0f));
			mCameraPosition += moveVec; moveVec = glm::vec3(0.0f);

			//Move square
			if (Input::IsKeyPressed(LAB_KEY_J))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(-1.0f, 0.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_L))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(1.0f, 0.0f, 0.0f));

			if (Input::IsKeyPressed(LAB_KEY_I))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(0.0f, 1.0f, 0.0f));
			else if (Input::IsKeyPressed(LAB_KEY_K))
				moveVec += (mTriangleMoveSpeed / mCameraZoom) * ts * (rotMat * glm::vec3(0.0f, -1.0f, 0.0f));
			mTrianglePos += moveVec;

			if (Input::IsKeyPressed(LAB_KEY_Q))
				mCameraRotation += mCameraRotationSpeed * ts;
			if (Input::IsKeyPressed(LAB_KEY_E))
				mCameraRotation -= mCameraRotationSpeed * ts;

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			mCamera.setPosition(mCameraPosition);
			mCamera.setRotation(mCameraRotation);
			mCamera.setZoom(mCameraZoom);

			Renderer::BeginState(mCamera);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			for (int y = 0; y < 20; y++)
			{
				for (int x = 0; x < 20; x++)
				{
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					Renderer::Send(mBlueShader, mSquareVA, transform);
				}
			}

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), mTrianglePos);

			Renderer::Send(mShader, mVertexArray, transform);
			Renderer::Send(mTriShader, mTriVA);

			Renderer::EndState();
		}

		void onEvent(Labyrinth::Event& event) override
		{
			if (event.getEventType() == Labyrinth::EventType::KeyPressed)
			{
				Labyrinth::KeyPressedEvent& e = (Labyrinth::KeyPressedEvent&)event;
				if (e.getKeyCode() == LAB_KEY_TAB)
					LAB_TRACE("Tab key is pressed (event)!");

				LAB_TRACE("{0}", e.getKeyName());
			}
			if (event.getEventType() == Labyrinth::EventType::MouseScrolled)
			{
				Labyrinth::MouseScrolledEvent& e = (Labyrinth::MouseScrolledEvent&)event;
				float zoomDelta = e.getYOffset() / 25.0f;
				mCameraZoom = (mCameraZoom + zoomDelta > 0.1f) ? mCameraZoom + zoomDelta : 0.1f; //Maximum zoom out of 10x
			}
		}

	private:
		Ref<Shader> mShader;
		Ref<VertexArray> mVertexArray;

		Ref<Shader> mBlueShader;
		Ref<VertexArray> mSquareVA;

		Ref<Shader> mTriShader;
		Ref<VertexArray> mTriVA;

		OrthographicCamera mCamera;
		glm::vec3 mCameraPosition;
		float mCameraMoveSpeed = 5.0f;
		float mCameraZoom = 1.0f;
		glm::mat3 mCameraMatRot;
		float mCameraRotation = 0.0f;
		float mCameraRotationSpeed = 180.0f;

		glm::vec3 mTrianglePos;
		float mTriangleMoveSpeed = 2.0f;
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