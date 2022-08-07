#pragma once

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/IO/Filesystem.h>

#include <string>

namespace Labyrinth {

	struct ProjectSettings
	{
		std::string name;

		fs::path assetDir;
		fs::path assetRegPath;

		fs::path startScenePath;

		fs::path scriptModulePath;

		// Not serialised
		fs::path projectFilename;
		fs::path projectDir;
	};

	class Project : public RefCounted
	{
	public:
		Project() = default;

		const ProjectSettings& getSettings() const { return mSettings; }

		static Ref<Project> GetActive() { return sActiveProject; }
		static void SetActive(Ref<Project> project);

		static const std::string& GetProjectName()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return sActiveProject->getSettings().name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return sActiveProject->getSettings().projectDir;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return std::filesystem::path(sActiveProject->getSettings().projectDir) / sActiveProject->getSettings().assetDir;
		}

		static std::filesystem::path GetAssetRegistryPath()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return std::filesystem::path(sActiveProject->getSettings().projectDir) / sActiveProject->getSettings().assetRegPath;
		}

		static std::filesystem::path GetScriptModulePath()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return std::filesystem::path(sActiveProject->getSettings().projectDir) / sActiveProject->getSettings().scriptModulePath;
		}

		static std::filesystem::path GetScriptModuleFilePath()
		{
			LAB_CORE_ASSERT(sActiveProject);
			return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
		}

	private:
		ProjectSettings mSettings;

		inline static Ref<Project> sActiveProject;

		friend class ProjectSerialiser;
		friend class OptionsModal;
	};
}