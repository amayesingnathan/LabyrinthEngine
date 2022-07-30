#include "Lpch.h"
#include "AssetSerialiser.h"
#include "AssetManager.h"

#include "Labyrinth/IO/YAML.h"
#include "Labyrinth/Renderer/Texture.h"
#include "Labyrinth/Renderer/SubTexture.h"
#include "Labyrinth/Scene/SceneSerialiser.h"
#include "Labyrinth/Tools/StringUtils.h"

namespace Labyrinth {

	bool TextureSerializer::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Texture2D::Create(AssetManager::GetFileSystemPathString(metadata));
		asset->handle = metadata.handle;

		bool result = asset.to<Texture2D>()->loaded();

		if (!result)
			asset->setFlag(AssetFlag::Invalid);

		return result;
	}

	void SubTextureSerializer::serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<SubTexture2D> subtex = asset.to<SubTexture2D>();

		YAML::Emitter out;
		out << YAML::BeginMap; // Subtexture
		out << YAML::Key << "SubTexture" << YAML::Value;
		{
			out << YAML::BeginMap;

			LAB_SERIALISE_PROPERTY_ASSET(TextureSheet, subtex->getSheet(), out);
			LAB_SERIALISE_PROPERTY(SubTexName, subtex->getName(), out);

			out << YAML::Key << "Coordinates" << YAML::Value << YAML::BeginSeq;
			glm::vec2* texCoords = subtex->getTexCoords();

			out << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(0, texCoords[0], out);
			out << YAML::EndMap;

			out << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(1, texCoords[1], out);
			out << YAML::EndMap;

			out << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(2, texCoords[2], out);
			out << YAML::EndMap;

			out << YAML::BeginMap;
			LAB_SERIALISE_PROPERTY(3, texCoords[3], out);
			out << YAML::EndMap;

			out << YAML::EndSeq; // Coordinates

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Subtexture
	}

	bool SubTextureSerializer::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
		if (!stream.is_open())
			return false;

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node root = YAML::Load(strStream.str());
		YAML::Node subtexNode = root["SubTexture"];

		Ref<Texture2DSheet> sheet = nullptr;
		std::string subtexName;
		glm::vec2 coords[4];

		glm::vec2 defaultCoord{ 0.f, 0.f };
		std::string defaultName;

		LAB_DESERIALISE_PROPERTY_ASSET(TextureSheet, sheet, subtexNode, Texture2DSheet);
		if (!sheet)
			return false;

		LAB_DESERIALISE_PROPERTY(SubTexName, subtexName, subtexNode, defaultName);

		auto coordsNode = subtexNode["Coordinates"];
		if (!coordsNode) return false;

		LAB_DESERIALISE_PROPERTY(0, coords[0], coordsNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY(1, coords[1], coordsNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY(2, coords[2], coordsNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY(3, coords[3], coordsNode, defaultCoord);

		asset = Ref<SubTexture2D>::Create(sheet, coords, subtexName);
		asset->handle = metadata.handle;

		return true;
	}

	void TextureSheetSerializer::serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Texture2DSheet> sheet = asset.to<Texture2DSheet>();

		YAML::Emitter out;
		out << YAML::BeginMap; // Subtexture
		out << YAML::Key << "SubTexture" << YAML::Value;
		{
			out << YAML::BeginMap;

			LAB_SERIALISE_PROPERTY_ASSET(Texture, sheet->getBaseTex(), out);
			LAB_SERIALISE_PROPERTY(Name, sheet->getName(), out);
			LAB_SERIALISE_PROPERTY(TileSize, sheet->getTileSize(), out);

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // TextureSheet
	}

	bool TextureSheetSerializer::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
		if (!stream.is_open())
			return false;

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node root = YAML::Load(strStream.str());
		YAML::Node texNode = root["Texture"];

		Ref<Texture2D> tex = nullptr;
		std::string texName;
		glm::vec2 tileSize;

		glm::vec2 defaultSize{ 1.f, 1.f };
		std::string defaultName;

		LAB_DESERIALISE_PROPERTY_ASSET(Texture, tex, texNode, Texture2D);
		if (!tex)
			return false;

		LAB_DESERIALISE_PROPERTY(Name, texName, texNode, defaultName);
		LAB_DESERIALISE_PROPERTY(TileSize, tileSize, texNode, defaultSize);

		asset = Ref<Texture2DSheet>::Create(tex, tileSize, texName);
		asset->handle = metadata.handle;

		return true;
	}

	void SceneAssetSerializer::serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Scene> scene = asset.to<Scene>(); 
		SceneSerialiser serialiser(scene);
		serialiser.serialise(AssetManager::GetFileSystemPath(metadata).string());
	}

	bool SceneAssetSerializer::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		Ref<Scene> scene = Ref<Scene>::Create();
		SceneSerialiser serialiser(scene);
		bool result = serialiser.deserialise(metadata.filepath.string());

		if (!result)
			return false;

		asset = scene;
		asset->handle = metadata.handle;
		return true;
	}
}