#include "Lpch.h"
#include "Project.h"

#include "Labyrinth/Assets/AssetManager.h"

namespace Labyrinth {

	void Project::SetActive(Ref<Project> project)
	{
		if (sActiveProject)
			AssetManager::Shutdown();

		sActiveProject = project;

		if (sActiveProject)
			AssetManager::Init();
	}

}