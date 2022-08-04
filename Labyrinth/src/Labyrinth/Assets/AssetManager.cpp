#include "Lpch.h"
#include "AssetManager.h"

#include "AssetExtensions.h"

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Tools/StringUtils.h>

namespace Labyrinth {

	static AssetMetadata sEmptyMetadata;

	void AssetManager::Init()
	{
		AssetImporter::Init();

		sAssetDirPath = Application::Get().getSpec().workingDir.string() + "assets";
		sAssetRegPath = sAssetDirPath / "AssetRegistry.lreg";
		LoadRegistry();
		ReloadAssets();
	}

	void AssetManager::Shutdown()
	{
		SaveRegistry();

		sMemoryAssets.clear();
		sAssetRegistry.clear();
		sLoadedAssets.clear();

		AssetImporter::Shutdown();
	}

	AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		std::string ext = Utils::String::ToLowerCopy(extension);
		if (sAssetExtensionMap.count(ext) == 0)
			return AssetType::None;

		return sAssetExtensionMap.at(ext.c_str());
	}

	AssetType AssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		return GetAssetTypeFromExtension(path.extension().string());
	}

	bool AssetManager::IsExtensionValid(const std::string& extension, AssetType type)
	{
		std::string ext = Utils::String::ToLowerCopy(extension);
		if (sAssetExtensionMap.count(ext) == 0)
			return false;
		return sAssetExtensionMap[ext] == type;
	}

	AssetHandle AssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		std::filesystem::path path = GetRelativePath(filepath);

		if (auto& metadata = GetMetadata(path); metadata.valid())
			return metadata.handle;

		AssetType type = GetAssetTypeFromPath(path);
		if (type == AssetType::None)
			return 0;

		AssetMetadata metadata;
		metadata.handle = AssetHandle();
		metadata.filepath = path;
		metadata.type = type;
		sAssetRegistry[metadata.handle] = metadata;

		return metadata.handle;
	}

	bool AssetManager::ReloadData(AssetHandle assetHandle)
	{
		auto& metadata = GetMetadataInternal(assetHandle);
		if (!metadata.valid())
		{
			LAB_CORE_ERROR("Trying to reload invalid asset");
			return false;
		}

		if (!metadata.dataLoaded)
		{
			LAB_CORE_WARN("Trying to reload asset that was never loaded");

			Ref<Asset> asset;
			metadata.dataLoaded = AssetImporter::Deserialise(metadata, asset);
			return metadata.dataLoaded;
		}

		LAB_CORE_ASSERT(sLoadedAssets.count(assetHandle) != 0);
		Ref<Asset>& asset = sLoadedAssets.at(assetHandle);
		metadata.dataLoaded = AssetImporter::Deserialise(metadata, asset);
		return metadata.dataLoaded;
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
			metadata.handle = entry["Handle"].as<u64>();
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

	void AssetManager::ProcessDirectory(const std::filesystem::path& directoryPath)
	{
		for (auto entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
				ProcessDirectory(entry.path());
			else
				ImportAsset(entry.path());
		}
	}

	void AssetManager::ReloadAssets()
	{
		ProcessDirectory(sAssetDirPath);
		SaveRegistry();
	}

	void AssetManager::SaveRegistry()
	{
		// Sort assets by UUID to make project managment easier
		struct AssetRegistryEntry
		{
			std::string filepath;
			AssetType type;
		};

		std::map<UUID, AssetRegistryEntry> sortedMap;
		for (auto& [filepath, metadata] : sAssetRegistry)
		{
			if (!std::filesystem::exists(GetFileSystemPath(metadata)))
				continue;

			if (metadata.memoryAsset)
				continue;

			std::string pathToSerialize = metadata.filepath.string();
#ifdef LAB_PLATFORM_WINDOWS
			std::replace(pathToSerialize.begin(), pathToSerialize.end(), '\\', '/');
#endif
			sortedMap[metadata.handle] = { pathToSerialize, metadata.type };
		}

		LAB_CORE_INFO("[AssetManager] serializing asset registry with {0} entries", sortedMap.size());

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (auto& [handle, entry] : sortedMap)
		{
			out << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(Handle, handle, out);
			LAB_SERIALISE_PROPERTY(Filepath, entry.filepath, out);
			LAB_SERIALISE_PROPERTY(Type, AssetUtils::AssetTypeToString(entry.type), out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(sAssetRegPath);
		fout << out.c_str();
	}

	AssetMetadata& AssetManager::GetMetadataInternal(AssetHandle handle)
	{
		if (sAssetRegistry.contains(handle))
			return sAssetRegistry[handle];

		return sEmptyMetadata; // make sure to check return value before you go changing it!
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
		if (temp.find(sAssetDirPath.string()) != std::string::npos)
		{
			relativePath = std::filesystem::relative(filepath, sAssetDirPath);
			if (relativePath.empty())
			{
				relativePath = filepath.lexically_normal();
			}
		}
		return relativePath;
	}


}