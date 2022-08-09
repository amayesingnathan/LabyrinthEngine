#include "NewProjectModal.h"

#include "../EditorLayer.h"

#include <Labyrinth/Core/Buffer.h>
#include <Labyrinth/Project/Project.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	NewProjectModal::NewProjectModal(EditorData& editorData)
		: mEditorData(editorData), mProjectPath(editorData.projectFilepath), mProjectName(editorData.projectName)
	{
		if (mProjectPath.empty()) mProjectPath = std::filesystem::current_path();
		if (mProjectName.empty()) mProjectName = "NewProject";
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
	}

	void NewProjectModal::onComplete()
	{
		mEditorData.projectFilepath = mProjectPath;
		mEditorData.projectName = mProjectName;
	}
}