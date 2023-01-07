#include "Lpch.h"
#include "Project.h"

#include "ProjectSerialiser.h"

#include "Labyrinth/Assets/AssetManager.h"

namespace Laby {

	static Ref<Project> sActiveProject = nullptr;

	void ActiveProject::Set(Ref<Project> newProj) { sActiveProject = newProj; }
	Ref<Project> ActiveProject::Get() { return sActiveProject; }

	bool Project::IsActive()
	{
		return sActiveProject;
	}

	void Project::SetInactive()
	{
		sActiveProject = nullptr;
	}

	void Project::Load(const fs::path& path)
	{
		Ref<Project> project = Ref<Project>::Create();
		ProjectSerialiser serialiser(project);
		serialiser.deserialise(path);
		sActiveProject = project;
	}

	void Project::Save(const fs::path& path)
	{
		ProjectSerialiser serialiser(sActiveProject);
		serialiser.serialise(path);
	}

	const std::string& Project::GetProjectName()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return sActiveProject->mSettings.name;
	}

	const fs::path& Project::GetProjectDirectory()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return sActiveProject->mSettings.projectDir;
	}

	fs::path Project::GetProjectPath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return fs::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.projectName;
	}

	fs::path Project::GetAssetDirectory()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return fs::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.assetDir;
	}

	fs::path Project::GetAssetRegistryPath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return fs::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.assetRegPath;
	}

	fs::path Project::GetScriptModulePath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return fs::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.scriptModulePath;
	}

	fs::path Project::GetScriptModuleFilePath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
	}

	fs::path Project::GetStartScenePath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return fs::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.startScenePath;
	}
}