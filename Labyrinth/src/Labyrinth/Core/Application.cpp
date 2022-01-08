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
			float time = (float)SDL_GetTicks() / 2000.0f;
			Timestep timestep = time - mLastFrameTime;
			mLastFrameTime = time;

			for (Layer* layer : mLayerStack)
				layer->onUpdate(timestep);

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
		//case LAB_KEY_W:
		//	mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.0f, -0.04f, 0.0f });
		//	return true;
		//case LAB_KEY_A:
		//	mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.04f, 0.0f, 0.0f });
		//	return true;
		//case LAB_KEY_S:
		//	mCamera.setPosition(mCamera.getPosition() + glm::vec3{ 0.0f, 0.04f, 0.0f });
		//	return true;
		//case LAB_KEY_D:
		//	mCamera.setPosition(mCamera.getPosition() + glm::vec3{ -0.04f, 0.0f, 0.0f });
		//	return true;
		//case LAB_KEY_Q:
		//	mCamera.setRotation(mCamera.getRotation() - 3.0f);
		//	return true;
		//case LAB_KEY_E:
		//	mCamera.setRotation(mCamera.getRotation() + 3.0f);
		//	return true;
		}
		return false;
	}
}
