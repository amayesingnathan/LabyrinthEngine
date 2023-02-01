#include "Lpch.h"
#include "ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <Labyrinth/Editor/EditorResources.h>

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

	bool ImGuiUtils::IsMouseDown(MouseCode button)
	{
		return ImGui::IsMouseDown(button);
	}

	bool ImGuiUtils::IsMouseReleased(MouseCode button)
	{
		return ImGui::IsMouseReleased(button);
	}

	glm::vec2 ImGuiUtils::CursorPos()
	{
		return ToGLM(ImGui::GetCursorPos());
	}

	void ImGuiUtils::SetCursorPos(const glm::vec2& pos)
	{
		ImGui::SetCursorPos(FromGLM(pos));
	}

	void ImGuiUtils::SetCursorPosX(f32 pos)
	{
		ImGui::SetCursorPosX(pos);
	}

	void ImGuiUtils::SetCursorPosY(f32 pos)
	{
		ImGui::SetCursorPosY(pos);
	}

	void ImGuiUtils::SetButtonTransparent()
	{
		auto& colours = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
		const auto& buttonActive = colours[ImGuiCol_ButtonActive];

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Button, FromGLM(EditorResources::ClearColour));
	}

	void ImGuiUtils::ResetButtonTransparency()
	{
		ImGui::PopStyleColor(3);
	}

	glm::vec2 ImGuiUtils::AvailableRegion()
	{
		return ToGLM(ImGui::GetContentRegionAvail());
	}

	glm::vec2 ImGuiUtils::AvailableRegionMax()
	{
		return ToGLM(ImGui::GetWindowContentRegionMax());
	}

	f32 ImGuiUtils::FontSize()
	{
		return GImGui->Font->FontSize;
	}

	glm::vec2 ImGuiUtils::FramePadding()
	{
		return ToGLM(GImGui->Style.FramePadding);
	}

	f32 ImGuiUtils::FrameHeightWithSpacing()
	{
		return ImGui::GetFrameHeightWithSpacing();
	}

	f32 ImGuiUtils::LineHeight()
	{
		return GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
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

	void ImGuiUtils::PushID(std::string_view strID)
	{
		ImGui::PushID(strID.data());
	}

	void ImGuiUtils::PopID()
	{
		ImGui::PopID();
	}

	void ImGuiUtils::PushStyle(ImGuiStyleVar flags, f32 var)
	{
		ImGui::PushStyleVar(flags, var);
	}

	void ImGuiUtils::PushStyle(ImGuiStyleVar flags, const glm::vec2& var)
	{
		ImGui::PushStyleVar(flags, FromGLM(var));
	}

	void ImGuiUtils::PopStyle(i32 count)
	{
		ImGui::PopStyleVar(count);
	}

	void ImGuiUtils::PushStyleColour(ImGuiCol flags, const glm::vec4& var)
	{
		ImGui::PushStyleColor(flags, FromGLM(var));
	}

	void ImGuiUtils::PopStyleColour()
	{
		ImGui::PopStyleColor();
	}
}