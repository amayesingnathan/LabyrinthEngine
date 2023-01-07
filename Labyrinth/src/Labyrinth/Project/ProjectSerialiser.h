#pragma once

#include "Project.h"

namespace Laby {

	class ProjectSerialiser
	{
	public:
		ProjectSerialiser();
		ProjectSerialiser(Ref<Project> project);

		void serialise(const fs::path& filepath);
		bool deserialise(const fs::path& filepath);

	private:
		Ref<Project> mProject;
	};
}