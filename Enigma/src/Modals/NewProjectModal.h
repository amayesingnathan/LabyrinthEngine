#pragma once

#include "Modal.h"

#include <Labyrinth/IO/Filesystem.h>

namespace Labyrinth {

	class NewProjectModal : public Modal
	{
	public:
		NewProjectModal(fs::path& projectPath, std::string& projectName);

		void onImGuiRender() override;

		static Ref<NewProjectModal> Create(fs::path& projectPath, std::string& projectName) { return Ref<NewProjectModal>::Create(projectPath, projectName); }

	private:
		void Save();

	private:
		fs::path& mProjectPath;
		std::string& mProjectName;
	};

}
