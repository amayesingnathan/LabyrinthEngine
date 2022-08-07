#include "Lpch.h"
#include "ProjectSerialiser.h"

#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/IO/YAML.h>

namespace Labyrinth {

    ProjectSerialiser::ProjectSerialiser(Ref<Project> project) 
        : mProject(project) 
    {
        
    }

    void ProjectSerialiser::serialise(const fs::path& filepath) 
    {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
        {
            out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << mProject->mSettings.name;
			out << YAML::Key << "AssetDirectory" << YAML::Value << mProject->mSettings.assetDir.string();
			out << YAML::Key << "AssetRegistry" << YAML::Value << mProject->mSettings.assetRegPath.string();
			out << YAML::Key << "ScriptModulePath" << YAML::Value << mProject->mSettings.scriptModulePath.string();
			out << YAML::Key << "StartScene" << YAML::Value << mProject->mSettings.startScenePath.string();

            out << YAML::EndMap;
        }

		FileUtils::Write(filepath, out.c_str());
    }

    bool ProjectSerialiser::deserialise(const fs::path& filepath) 
    {
		YAML::Node data = YAML::Load(FileUtils::Read(filepath).as<char>());
		if (!data["Project"])
			return false;

		YAML::Node rootNode = data["Project"];
		if (!rootNode["Name"])
			return false;

		auto& config = mProject->mSettings;
		config.name = rootNode["Name"].as<std::string>();

		config.assetDir = rootNode["AssetDirectory"].as<std::string>();
		config.assetRegPath = rootNode["AssetRegistry"].as<std::string>();

		config.scriptModulePath = rootNode["ScriptModulePath"].as<std::string>();

		config.startScenePath = rootNode["StartScene"] ? rootNode["StartScene"].as<std::string>() : "";

		return true;

	}
}
