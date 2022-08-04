#include "Lpch.h"
#include "Window.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"
#include "Labyrinth/Events/KeyEvent.h"

#include "Labyrinth/Renderer/Renderer.h"

#include "OpenGL/OpenGLContext.h"

#include "GLFW//glfw3.h"

namespace Labyrinth {

	static u8 sGLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LAB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Single<Window> Window::Create(const WindowProps& props)
	{
		return CreateSingle<Window>(props);
	}

	Window::Window(const WindowProps& props)
	{
		LAB_PROFILE_FUNCTION();

		init(props);
	}

	Window::~Window()
	{
		LAB_PROFILE_FUNCTION();

		shutdown();
	}

	void Window::init(const WindowProps& props)
	{
		LAB_PROFILE_FUNCTION();

		mData.title = props.title;
		mData.width = props.width;
		mData.height = props.height;

		if (sGLFWWindowCount == 0)
		{
			LAB_PROFILE_SCOPE("glfwInit");

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
			LAB_PROFILE_SCOPE("glfwCreateWindow");
#if defined(LAB_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
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

				WindowResizeEvent event(width, height);
				data.eventCallback(event);
			});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.eventCallback(event);
			});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.eventCallback(event);
			});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, f64 xOffset, f64 yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((f32)xOffset, (f32)yOffset);
				data.eventCallback(event);
			});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, f64 xPos, f64 yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((f32)xPos, (f32)yPos);
				data.eventCallback(event);
			});
	}

	void Window::shutdown()
	{
		LAB_PROFILE_FUNCTION();

		glfwDestroyWindow(mWindow);
		--sGLFWWindowCount;

		if (sGLFWWindowCount == 0)
		{
			glfwTerminate();
		}
		LAB_CORE_TRACE("Shutdown complete");
	}


	void Window::onUpdate()
	{
		LAB_PROFILE_FUNCTION();

		glfwPollEvents();
		mContext->swapBuffers();
	}

	void Window::setTitle(const std::string& title)
	{
		mData.title = title;
		glfwSetWindowTitle(mWindow, title.c_str());
	}

	void Window::setVSync(bool enabled)
	{
		LAB_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.vSync = enabled;
	}

	bool Window::isVSync() const
	{
		return mData.vSync;
	}
}