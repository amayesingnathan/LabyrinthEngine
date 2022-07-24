#include "Lpch.h"
#include "AssetManager.h"

#include <Labyrinth/Tools/StringUtils.h>

#include "yaml-cpp/yaml.h"

namespace Labyrinth {

	static AssetMetadata sEmptyMetadata;

	void AssetManager::Init()
	{
		LoadRegistry();
	}

	void AssetManager::Shutdown()
	{
		SaveRegistry();

		sMemoryAssets.clear();
		sAssetRegistry.clear();
		sLoadedAssets.clear();
	}

	void AssetManager::LoadRegistry()
	{
		if (!std::filesystem::exists(sAssetRegPath))
			return;

		std::ifstream stream(sAssetRegPath);
		LAB_CORE_ASSERT(stream);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		auto handles = data["Assets"];
		if (!handles)
		{
			LAB_CORE_ERROR("[AssetManager] Asset Registry appears to be corrupted!");
			return;
		}

		for (auto entry : handles)
		{
			std::string filepath = entry["Filepath"].as<std::string>();

			AssetMetadata metadata;
			metadata.handle = entry["Handle"].as<uint64_t>();
			metadata.filepath = filepath;
			metadata.type = (AssetType)AssetUtils::AssetTypeFromString(entry["Type"].as<std::string>());

			if (metadata.type == AssetType::None)
				continue;

			if (!std::filesystem::exists(AssetManager::GetFileSystemPath(metadata)))
			{
				LAB_CORE_WARN("[AssetManager] Missing asset '{0}' detected in registry file", metadata.filepath);
				continue;
			}

			if (metadata.handle == 0)
			{
				LAB_CORE_WARN("[AssetManager] AssetHandle for {0} is 0, this shouldn't happen.", metadata.filepath);
				continue;
			}

			sAssetRegistry[metadata.handle] = metadata;
		}
	}

	void AssetManager::SaveRegistry()
	{
	}

	const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
	{
		if (sAssetRegistry.contains(handle))
			return sAssetRegistry[handle];

		return sEmptyMetadata;
	}

	const AssetMetadata& AssetManager::GetMetadata(const std::filesystem::path filepath)
	{
		const auto relativePath = GetRelativePath(filepath);

		for (auto& [handle, metadata] : sAssetRegistry)
		{
			if (metadata.filepath == relativePath)
				return metadata;
		}

		return sEmptyMetadata;
	}

	AssetHandle AssetManager::GetHandleFromPath(const std::filesystem::path& filepath)
	{
		return GetMetadata(filepath).handle;
	}

	std::filesystem::path AssetManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path relativePath = filepath.lexically_normal();
		std::string temp = filepath.string();
		if (temp.find(sAssetRegPath.string()) != std::string::npos)
		{
			relativePath = std::filesystem::relative(filepath, sAssetRegPath);
			if (relativePath.empty())
			{
				relativePath = filepath.lexically_normal();
			}
		}
		return relativePath;
	}


}