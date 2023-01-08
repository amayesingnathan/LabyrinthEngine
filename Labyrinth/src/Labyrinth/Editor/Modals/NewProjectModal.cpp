#include "Lpch.h"
#include "NewProjectModal.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>
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
		Widgets::Label("Full Project Path: %s", fullProjectPath.string().c_str());

		Widgets::StringEdit("Name", mProjectName);
		Widgets::PathEdit("Path", mProjectPath);

		Widgets::SameLine();

		Widgets::Button("...", [&, this]()
		{
			fs::path result = FileUtils::OpenDir();
			if (!result.empty())
				mProjectPath = result;
		});
	}

	void NewProjectModal::onComplete()
	{
		mEditorData.newProjectFilepath = mProjectPath;
		mEditorData.newProjectName = mProjectName;
	}
}