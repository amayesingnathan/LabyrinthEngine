#include "Lpch.h"
#include "Project.h"

#include "ProjectSerialiser.h"

#include "Labyrinth/Assets/AssetManager.h"

namespace Laby {

	static Ref<Project> sActiveProject = nullptr;

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

	std::filesystem::path Project::GetProjectDirectory()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return sActiveProject->mSettings.projectDir;
	}

	std::filesystem::path Project::GetAssetDirectory()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return std::filesystem::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.assetDir;
	}

	std::filesystem::path Project::GetAssetRegistryPath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return std::filesystem::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.assetRegPath;
	}

	std::filesystem::path Project::GetScriptModulePath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return std::filesystem::path(sActiveProject->mSettings.projectDir) / sActiveProject->mSettings.scriptModulePath;
	}

	std::filesystem::path Project::GetScriptModuleFilePath()
	{
		LAB_CORE_ASSERT(sActiveProject);
		return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
	}
}