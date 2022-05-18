#include "Lpch.h"
#include "Application.h"

#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Renderer/Renderer.h"
#include "Labyrinth/IO/Input.h"
#include "Labyrinth/Tools/PlatformUtils.h"

namespace Labyrinth {

	Application* Application::sInstance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		: mCommandLineArgs(args)
	{
		LAB_PROFILE_FUNCTION();

		LAB_CORE_ASSERT(!sInstance, "Application already exists");
		sInstance = this;

		mWindow = Window::Create(WindowProps(name));
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

	void Application::Close()
	{
		if (mBlockExit) return;

		mRunning = false;
	}

	void Application::run()
	{
		LAB_PROFILE_FUNCTION(); 

		while (mRunning)
		{
			LAB_PROFILE_SCOPE("RunLoop");

			float time = Stopwatch::GetTime();
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

		// Check for Esc after in case an event in a layer handles it.
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(Application::OnKeyPressed));
	}

	void Application::pushLayer(Layer* layer)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushLayer(layer);
	}

	void Application::pushNetLayer(NetworkLayer* netlayer)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushLayer(netlayer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		LAB_PROFILE_FUNCTION();
		mLayerStack.pushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
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
	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.getKeyCode() == Key::Escape)
		{
			Close();
		}

		return false;
	}
}
