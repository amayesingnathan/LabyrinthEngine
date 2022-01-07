#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Platforms/Windows/WindowsWindow.h"

#include "Input.h"

#include "SDL.h"
#include "Glad/glad.h"
#include "SDL_opengl.h"

namespace Labyrinth {

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	static GLenum ShaderDataTypetoOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}
	}

	Application::Application()
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		mWindow = Window::Create();
		mWindow->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));

		mImGuiLayer = new ImGuiLayer();
		pushOverlay(mImGuiLayer);

		//glGenVertexArrays(1, &mVertexArray);
		glCreateVertexArrays(1, &mVertexArray);
		glBindVertexArray(mVertexArray);

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		mVertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		mVertexBuffer->bind();

		{
			BufferLayout layout = {
				{ShaderDataType::Float3, "aPosition"},
				{ShaderDataType::Float4, "aColour"}
			};

			mVertexBuffer->setLayout(layout);
		}

		uint32_t index = 0;
		const auto& layout = mVertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, 
				element.getComponentCount(), 
				ShaderDataTypetoOpenGLType(element.type), 
				element.normalised ? GL_TRUE : GL_FALSE, 
				layout.getStride(),
				(const void*)element.offset);
			index++;
		}

		uint32_t indices[3] = { 0, 1, 2 };

		mIndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		mIndexBuffer->bind();

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColour;

			out vec3 vPosition;
			out vec4 vColour;

			void main()
			{
				vPosition = aPosition;
				vColour = aColour;
				gl_Position = vec4(aPosition, 1.0);	
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
	}

	void Application::run()
	{
		mRunning = true;

		while (mRunning)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			mShader->bind();
			glBindVertexArray(mVertexArray);
			glDrawElements(GL_TRIANGLES, mIndexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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
		case SDL_SCANCODE_ESCAPE:
			mRunning = false;
			return true;
		}
		return false;
	}
}
