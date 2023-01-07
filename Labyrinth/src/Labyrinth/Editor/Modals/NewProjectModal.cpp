#include "Lpch.h"
#include "NewProjectModal.h"

#include <imgui.h>

#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/Project/Project.h>

#include "../EditorData.h"

namespace Laby {

	NewProjectModal::NewProjectModal(EditorData& editorData)
		: mEditorData(editorData), mProjectPath(editorData.newProjectFilepath), mProjectName(editorData.newProjectName)
	{
		if (mProjectPath.empty()) 
			mProjectPath = std::filesystem::current_path();

		if (mProjectName.empty()) 
			mProjectName = "NewProject";
	}

	void NewProjectModal::onImGuiRender()
	{
		fs::path fullProjectPath = mProjectPath / mProjectName;
		ImGui::Text("Full Project Path: %s", fullProjectPath.string().c_str());

		StaticString<256> nameBuffer(mProjectName);
		if (ImGui::InputText("Name", nameBuffer, nameBuffer.length()))
			mProjectName = nameBuffer.toString();

		StaticString<256> pathBuffer(mProjectPath.string());
		if (ImGui::InputText("Path", pathBuffer, pathBuffer.length()))
			mProjectPath = pathBuffer.toString();

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
		mEditorData.newProjectFilepath = mProjectPath;
		mEditorData.newProjectName = mProjectName;
	}
}