#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/Log.h"
#include "Platforms/Windows/WindowsWindow.h"

#include "Labyrinth/Renderer/Renderer.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Labyrinth {

	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		mWindow = Window::Create();
		mWindow->setEventCallback(LAB_BIND_EVENT_FUNC(Application::onEvent));

		Renderer::Init();

		mImGuiLayer = new ImGuiLayer();
		pushOverlay(mImGuiLayer);

	}

	Application::~Application()
	{
		Renderer::Shutdown();
	}

	void Application::run()
	{
		mRunning = true;

		while (mRunning)
		{
			if (Input::IsKeyPressed(LAB_KEY_ESCAPE))
				mRunning = false;

			float time = (float)SDL_GetTicks();
			Timestep timestep = time - mLastFrameTime;
			mLastFrameTime = time;

			if (!mMinimised)
			{
				for (Layer* layer : mLayerStack)
					layer->onUpdate(timestep);
			}

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
		dispatcher.dispatch<WindowCloseEvent>(LAB_BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(Application::OnWindowResize));

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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			mMinimised = true;
			return false;
		}

		mMinimised = false;
		Renderer::OnWindowResize(e.getWidth(), e.getHeight());
		return false;
	}
}
