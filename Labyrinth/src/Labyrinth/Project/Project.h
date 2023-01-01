#pragma once

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/IO/Filesystem.h>

#include <string>

namespace Laby {

	struct ProjectSettings
	{
		std::string name = "NewProject";

		fs::path assetDir;
		fs::path assetRegPath;

		fs::path startScenePath;

		fs::path scriptModulePath;

		// Not serialised
		std::string projectName;
		fs::path projectDir;
	};

	class Project : public RefCounted
	{
	public:
		Project() = default;

		static bool IsActive();
		static void SetInactive();

		static void Load(const fs::path& path);
		static void Save(const fs::path& path);

		static const std::string& GetProjectName();
		static std::filesystem::path GetProjectDirectory();
		static std::filesystem::path GetAssetDirectory();
		static std::filesystem::path GetAssetRegistryPath();
		static std::filesystem::path GetScriptModulePath();
		static std::filesystem::path GetScriptModuleFilePath();

	private:
		ProjectSettings mSettings;

		friend class ProjectSerialiser;
		friend class ProjectSettingsModal;
	};
}