#pragma once

#include "Modal.h"

#include <Labyrinth/Project/Project.h>
#include <Labyrinth/Scene/Scene.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class ProjectSettingsModal : public Modal
	{
	public:
		ProjectSettingsModal(const Ref<Project>& project);

		void onImGuiRender() override;

		static Ref<ProjectSettingsModal> Create(const Ref<Project>& project) { return Ref<ProjectSettingsModal>::Create(project); }

	private:
		Ref<Project> mProject;
	};

}
