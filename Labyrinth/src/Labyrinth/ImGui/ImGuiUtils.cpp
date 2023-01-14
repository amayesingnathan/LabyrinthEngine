#include "Lpch.h"
#include "ImGuiUtils.h"

#include <imgui.h>

namespace Laby {

	const glm::vec2& ImGuiUtils::ToGLM(const ImVec2& vec)
	{
		return *(glm::vec2*)&vec;
	}

	const glm::vec3& ImGuiUtils::ToGLM(const ImVec3& vec)
	{
		return *(glm::vec3*)&vec;
	}

	const glm::vec4& ImGuiUtils::ToGLM(const ImVec4& vec)
	{
		return *(glm::vec4*)&vec;
	}

	const ImVec2& ImGuiUtils::FromGLM(const glm::vec2& vec)
	{
		return *(ImVec2*)&vec;
	}

	const ImVec3& ImGuiUtils::FromGLM(const glm::vec3& vec)
	{
		return *(ImVec3*)&vec;
	}

	const ImVec4& ImGuiUtils::FromGLM(const glm::vec4& vec)
	{
		return *(ImVec4*)&vec;
	}

	f32 ImGuiUtils::WindowWidth()
	{
		return ImGui::GetWindowWidth();
	}

	f32 ImGuiUtils::WindowHeight()
	{
		return ImGui::GetWindowHeight();
	}

	void ImGuiUtils::PushItemWidth(f32 width)
	{
		ImGui::PushItemWidth(width);
	}

	void ImGuiUtils::PopItemWidth()
	{
		ImGui::PopItemWidth();
	}
}