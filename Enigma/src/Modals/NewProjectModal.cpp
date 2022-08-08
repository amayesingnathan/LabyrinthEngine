#include "NewProjectModal.h"

#include <Labyrinth/Core/Buffer.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	NewProjectModal::NewProjectModal(fs::path& projectPath, std::string& projectName)
		: mProjectPath(projectPath), mProjectName(projectName)
	{
		mProjectPath = std::filesystem::current_path();
		mProjectName = "NewProject";
	}

	void NewProjectModal::onImGuiRender()
	{
		fs::path fullProjectPath = mProjectPath / mProjectName;
		ImGui::Text("Full Project Path: %s", fullProjectPath.string().c_str());

		StaticBuffer<256> nameBuffer(mProjectName);
		if (ImGui::InputText("Name", nameBuffer, nameBuffer.size()))
			mProjectName = nameBuffer.string();

		StaticBuffer<256> pathBuffer(mProjectPath.string());
		if (ImGui::InputText("Path", pathBuffer, pathBuffer.size()))
			mProjectPath = pathBuffer.string();

		ImGui::SameLine();

		if (ImGui::Button("..."))
		{
			fs::path result = FileUtils::OpenDir();
			if (!result.empty())
				mProjectPath = result;
		}

		ImGui::Separator();

		if (ImGui::Button("OK"))
			Close();

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			mProjectPath = fs::path();
			mProjectName = std::string();
			Close();
		}
	}
}