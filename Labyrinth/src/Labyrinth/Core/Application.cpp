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
		LAB_PROFILE_FUNCTION();

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
		LAB_PROFILE_FUNCTION(); 
		Renderer::Shutdown();
	}

	void Application::run()
	{
		LAB_PROFILE_FUNCTION();

		while (mRunning)
		{
			LAB_PROFILE_SCOPE("RunLoop");
			if (Input::IsKeyPressed(LAB_KEY_ESCAPE))
				mRunning = false;

			float time = (float)SDL_GetTicks();
			Timestep timestep = time - mLastFrameTime;
			mLastFrameTime = time;

			if (!mMinimised)
			{
				LAB_PROFILE_SCOPE("LayerStack Update");

				for (Layer* layer : mLayerStack)
					layer->onUpdate(timestep);
			}

			mImGuiLayer->begin();
			{
				LAB_PROFILE_SCOPE("LayerStack ImGuiRender");

				for (Layer* layer : mLayerStack)
					layer->onImGuiRender();
			}
			mImGuiLayer->end();

			mWindow->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();

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
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		LAB_PROFILE_FUNCTION();

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
