#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Platforms/Windows/WindowsWindow.h"

#include "Labyrinth/Renderer/Renderer.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Labyrinth {

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	Application::Application()
		:mCamera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		mWindow = Window::Create();
		mWindow->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));

		mImGuiLayer = new ImGuiLayer();
		pushOverlay(mImGuiLayer);

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
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->setLayout({
			{ ShaderDataType::Float3, "a_Position" }
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

			out vec3 vPosition;
			out vec4 vColour;

			void main()
			{
				vPosition = aPosition;
				vColour = aColour;
				gl_Position = uViewProjection * vec4(aPosition, 1.0);	
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

			void main()
			{
				vPosition = aPosition;
				gl_Position = uViewProjection * vec4(aPosition, 1.0);	
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
	}

	void Application::run()
	{
		mRunning = true;

		while (mRunning)
		{
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			Renderer::BeginState(mCamera);

			Renderer::Send(mBlueShader, mSquareVA);
			Renderer::Send(mShader, mVertexArray);

			Renderer::EndState();

			for (Layer* layer : mLayerStack)
				layer->onUpdate();

			mImGuiLayer->begin();
			for (Layer* layer : mLayerStack)
				layer->onImGuiRender();
			mImGuiLayer->end();

			mWindow->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(Application::OnKeyPress));

		for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); it++)
		{
			if (e.handled)
				break;
			(*it)->onEvent(e);
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		mLayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		mLayerStack.pushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnKeyPress(KeyPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case LAB_KEY_ESCAPE:
			mRunning = false;
			return true;
		case LAB_KEY_W:
			mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.0f, -0.04f, 0.0f });
			return true;
		case LAB_KEY_A:
			mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.04f, 0.0f, 0.0f });
			return true;
		case LAB_KEY_S:
			mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.0f, 0.04f, 0.0f });
			return true;
		case LAB_KEY_D:
			mCamera.setPosition(mCamera.getPosition() + glm::vec3{ -0.04f, 0.0f, 0.0f });
			return true;
		case LAB_KEY_Q:
			mCamera.setRotation(mCamera.getRotation() - 3.0f);
			return true;
		case LAB_KEY_E:
			mCamera.setRotation(mCamera.getRotation() + 3.0f);
			return true;
		}
		return false;
	}
}
