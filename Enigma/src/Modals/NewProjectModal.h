#pragma once

#include <Labyrinth/Editor/EditorModal.h>

#include <Labyrinth/IO/Filesystem.h>

namespace Labyrinth {

	struct EditorData;

	class NewProjectModal : public EditorModal
	{
	public:
		NewProjectModal(EditorData& editorData);

		void onImGuiRender() override;
		void onComplete() override;

		static Ref<NewProjectModal> Create(EditorData& editorData) { return Ref<NewProjectModal>::Create(editorData); }


	private:
		EditorData& mEditorData;
		fs::path mProjectPath;
		std::string mProjectName;
	};

}
