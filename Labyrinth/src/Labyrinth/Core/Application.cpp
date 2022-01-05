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

	Application::Application()
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		mWindow = Window::Create();
		mWindow->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));

		mImGuiLayer = new ImGuiLayer();
		pushOverlay(mImGuiLayer);

		glGenVertexArrays(1, &mVertexArray);
		glBindVertexArray(mVertexArray);

		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &mIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
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

			glBindVertexArray(mVertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
