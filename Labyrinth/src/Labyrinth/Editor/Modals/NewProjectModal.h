#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

#include <Labyrinth/IO/Filesystem.h>

namespace Laby {

	struct EditorData;

	class NewProjectModal : public IEditorModal
	{
	public:
		NewProjectModal(EditorData& editorData);

		void OnRender() override;
		void OnComplete() override;

	private:
		EditorData& mEditorData;
		fs::path mProjectPath;
		std::string mProjectName;
	};

}
