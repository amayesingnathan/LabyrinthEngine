#include "Lpch.h"
#include "Window.h"

#include "GLFW/glfw3.h"

#include <Labyrinth/Events/EventManager.h>

namespace Laby {

	static u8 sGLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LAB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Single<Window> Window::Create(const WindowProps& props)
	{
		return MakeSingle<Window>(props);
	}

	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::onUpdate()
	{
	}

	void Window::setTitle(const std::string& title)
	{
	}

	void Window::setVSync(bool enabled)
	{
	}

	bool Window::isVSync() const
	{
		return false;
	}

	void Window::Init(const WindowProps& props)
	{
		mData.title = props.title;
		mData.width = props.width;
		mData.height = props.height;

		if (sGLFWWindowCount == 0)
		{
			int success = glfwInit();
			LAB_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		if (props.fullscreen)
		{	// Set Window size to be full size of primary monitor and set borderless (prefer this to actual fullscreen)
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			mData.width = mode->width;
			mData.height = mode->height;
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		}

		LAB_CORE_INFO("Creating window {0} ({1}, {2})", mData.title, mData.width, mData.height);

		{
#if defined(LAB_DEBUG)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			mWindow = glfwCreateWindow((int)mData.width, (int)mData.height, mData.title.c_str(), nullptr, nullptr);
			++sGLFWWindowCount;
		}
		LAB_CORE_ASSERT(mWindow, "Could not create GLFW window!");

		mContext = GraphicsContext::Create(mWindow);
		mContext->init();

		glfwSetWindowUserPointer(mWindow, &mData);
		setVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			EventManager::Post<WindowResizeEvent>(width, height);
		});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventManager::Post<WindowCloseEvent>();
		});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				EventManager::Post<KeyPressedEvent>((KeyCode)key, false);
				break;
			}
			case GLFW_RELEASE:
			{
				EventManager::Post<KeyReleasedEvent>((KeyCode)key);
				break;
			}
			case GLFW_REPEAT:
			{
				EventManager::Post<KeyPressedEvent>((KeyCode)key, true);
				break;
			}
			}
		});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		
			EventManager::Post<KeyTypedEvent>(keycode);
		});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				EventManager::Post<MouseButtonPressedEvent>(button);
				break;
			}
			case GLFW_RELEASE:
			{
				EventManager::Post<MouseButtonReleasedEvent>(button);
				break;
			}
			}
		});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, f64 xOffset, f64 yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventManager::Post<MouseScrolledEvent>((f32)xOffset, (f32)yOffset);
		});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, f64 xPos, f64 yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		
			EventManager::Post<MouseMovedEvent>((f32)xPos, (f32)yPos);
		});
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(mWindow);
		--sGLFWWindowCount;

		if (sGLFWWindowCount == 0)
		{
			glfwTerminate();
		}
		LAB_CORE_TRACE("Shutdown complete");
	}
}