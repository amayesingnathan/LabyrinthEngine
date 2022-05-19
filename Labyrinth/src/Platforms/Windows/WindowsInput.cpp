#include "Lpch.h"
#include "Labyrinth/IO/Input.h"

#include "Labyrinth/Core/Application.h"

#include <GLFW/glfw3.h>

#ifdef	LAB_PLATFORM_WINDOWS  //Input redefine for Windows!

namespace Labyrinth {

	bool Input::IsWindowHovered()
	{
		auto winSize = Application::Get().getWindow().getSize();
		auto pos = GetMousePosition();
		return ((pos.x > 0) && (pos.y > 0) && (pos.x < winSize.x) && (pos.y < winSize.y));
	}

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(KeyCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}

#endif