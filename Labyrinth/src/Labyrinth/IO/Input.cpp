#include "Lpch.h"
#include "Labyrinth/IO/Input.h"

#include "Labyrinth/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Laby {

	bool Input::IsWindowHovered()
	{
		auto winWidth = Application::Get().GetWindow().GetWidth();
		auto winHeight = Application::Get().GetWindow().GetHeight();
		auto pos = GetMousePosition();
		return ((pos.x > 0) && (pos.y > 0) && (pos.x < winWidth) && (pos.y < winHeight));
	}

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		f64 xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { static_cast<f32>(xpos), static_cast<f32>(ypos) };
	}

	f32 Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	f32 Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}