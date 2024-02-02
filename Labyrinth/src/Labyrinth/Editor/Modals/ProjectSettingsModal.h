#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

#include <Labyrinth/Project/Project.h>

namespace Laby {

	class ProjectSettingsModal : public IEditorModal
	{
	public:
		ProjectSettingsModal(const Ref<Project>& project);

		void OnRender() override;

	private:
		Ref<Project> mProject;
	};

}
