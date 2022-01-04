#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Labyrinth/Platforms/Windows/WindowsWindow.h"

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
	}

	void Application::run()
	{
		mRunning = true;

		while (mRunning)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

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
