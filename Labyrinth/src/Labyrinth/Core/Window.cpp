#include "Lpch.h"
#include "Window.h"

#include "GLFW/glfw3.h"


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
			mWindow = glfwCreateWindow((int)mData.width, (int)mData.height, mData.title.c_str(), nullptr, nullptr);
			++sGLFWWindowCount;
		}
		LAB_CORE_ASSERT(mWindow, "Could not create GLFW window!");

		mContext = GraphicsContext::Create(mWindow);
		mContext->init();

		glfwSetWindowUserPointer(mWindow, &mData);
		setVSync(true);
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