#include "Lpch.h"
#include "AssetSerialiser.h"
#include "AssetManager.h"

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/IO/YAML.h>
#include <Labyrinth/Renderer/Texture.h>
#include <Labyrinth/Renderer/SubTexture.h>
#include <Labyrinth/Scene/SceneSerialiser.h>
#include <Labyrinth/Tilemap/Tilemap.h>
#include <Labyrinth/Tools/StringUtils.h>

namespace Laby {

	bool TextureSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Ref<Texture2D>::Create(AssetManager::GetFileSystemPathString(metadata));
		asset->handle = metadata.handle;

		bool result = asset.to<Texture2D>()->loaded();

		if (!result)
			asset->setFlag(AssetFlag::Invalid);

		return result;
	}

	void SubTextureSerialiser::serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<SubTexture2D> subtex = asset.to<SubTexture2D>();

		YAML::Emitter out;
		out << YAML::BeginMap; // SubTexture
		out << YAML::Key << "SubTexture" << YAML::Value;
		{
			out << YAML::BeginMap;

			LAB_SERIALISE_PROPERTY_ASSET(TextureSheet, subtex->getSheet(), out);
			LAB_SERIALISE_PROPERTY(SheetLocation, subtex->getPosition(), out);

			const glm::vec2* texCoords = subtex->getTextureCoords();

			LAB_SERIALISE_PROPERTY(0, texCoords[0], out);
			LAB_SERIALISE_PROPERTY(1, texCoords[1], out);
			LAB_SERIALISE_PROPERTY(2, texCoords[2], out);
			LAB_SERIALISE_PROPERTY(3, texCoords[3], out);

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // SubTexture

		FileUtils::Write(AssetManager::GetFileSystemPath(metadata), out.c_str());
	}

	bool SubTextureSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		fs::path assetPath = AssetManager::GetFileSystemPath(metadata);
		std::string str;
		FileUtils::Read(assetPath, str);
		YAML::Node root = YAML::Load(str);
		YAML::Node subtexNode = root["SubTexture"];

		Ref<Texture2DSheet> sheet = nullptr;
		GridPosition pos;
		glm::vec2 coords[4];

		glm::vec2 defaultCoord{ 0.f, 0.f };

		LAB_DESERIALISE_PROPERTY_ASSET(TextureSheet, sheet, subtexNode, Texture2DSheet);
		LAB_DESERIALISE_PROPERTY(SheetLocation, pos, subtexNode);
		if (!sheet)
			return false;

		LAB_DESERIALISE_PROPERTY_DEF(0, coords[0], subtexNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY_DEF(1, coords[1], subtexNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY_DEF(2, coords[2], subtexNode, defaultCoord);
		LAB_DESERIALISE_PROPERTY_DEF(3, coords[3], subtexNode, defaultCoord);

		asset = Ref<SubTexture2D>::Create(sheet, pos, coords);
		asset->handle = metadata.handle;

		return true;
	}

	void TextureSheetSerialiser::serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Texture2DSheet> sheet = asset.to<Texture2DSheet>();

		fs::path sheetPath = "assets" / metadata.filepath.parent_path();
		if (!fs::exists(sheetPath))
			fs::create_directories(sheetPath);

		fs::create_directories(sheetPath / "subtextures");

		YAML::Emitter out;
		out << YAML::BeginMap; // TextureSheet
		out << YAML::Key << "TextureSheet" << YAML::Value;
		out << YAML::BeginMap;

		LAB_SERIALISE_PROPERTY_ASSET(Texture, sheet->getBaseTex(), out);
		LAB_SERIALISE_PROPERTY(Name, sheet->getName(), out);
		LAB_SERIALISE_PROPERTY(TileSize, sheet->getTileSize(), out);

		out << YAML::Key << "SubTextures";
		out << YAML::Value << YAML::BeginSeq;
		for (AssetHandle handle : sheet->getSubTextures())
			out << handle;
		out << YAML::EndSeq;
		out << YAML::EndMap; // SubTextures

		out << YAML::EndMap; // TextureSheet

		FileUtils::Write(AssetManager::GetFileSystemPath(metadata), out.c_str());
	}

	bool TextureSheetSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		fs::path assetPath = AssetManager::GetFileSystemPath(metadata);
		std::string str;
		FileUtils::Read(assetPath, str);
		YAML::Node root = YAML::Load(str);
		YAML::Node texNode = root["TextureSheet"];

		Ref<Texture2D> tex = nullptr;
		std::string texName;
		glm::vec2 tileSize;

		glm::vec2 defaultSize{ 1.f, 1.f };

		LAB_DESERIALISE_PROPERTY_ASSET(Texture, tex, texNode, Texture2D);
		if (!tex)
			return false;

		LAB_DESERIALISE_PROPERTY(Name, texName, texNode);
		LAB_DESERIALISE_PROPERTY(TileSize, tileSize, texNode);

		Ref<Texture2DSheet> sheet = Ref<Texture2DSheet>::Create(texName, tex, tileSize);

		auto subTextures = texNode["SubTextures"];
		for (auto subTexNode : subTextures)
			sheet->mSubTextures.emplace_back(subTexNode.as<AssetHandle>());

		if (sheet->mSubTextures.empty())
			sheet->generateTileset();

		asset = sheet;
		asset->handle = metadata.handle;

		return true;
	}

	void SceneAssetSerialiser::serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Scene> scene = asset.to<Scene>();
		SceneSerialiser serialiser(scene);
		serialiser.serialise(AssetManager::GetFileSystemPath(metadata));
	}

	bool SceneAssetSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		Ref<Scene> scene = Ref<Scene>::Create("Untitled");
		SceneSerialiser serialiser(scene);
		bool result = serialiser.deserialise(AssetManager::GetFileSystemPath(metadata));

		if (!result)
			return false;

		asset = scene;
		asset->handle = metadata.handle;
		return true;
	}

	void TilemapSerialiser::serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Tilemap> tilemap = asset.to<Tilemap>();

		fs::path sheetPath = "assets" / metadata.filepath.parent_path();
		if (!fs::exists(sheetPath))
			fs::create_directories(sheetPath);

		YAML::Emitter out;
		out << YAML::BeginMap; // Tilemap
		out << YAML::Key << "Tilemap" << YAML::Value;
		{
			out << YAML::BeginMap;

			LAB_SERIALISE_PROPERTY(Name, tilemap->getName(), out);
			LAB_SERIALISE_PROPERTY(Width, tilemap->getWidth(), out);
			LAB_SERIALISE_PROPERTY(Height, tilemap->getHeight(), out);

			out << YAML::Key << "SpriteSheets";
			out << YAML::Value << YAML::BeginSeq;
			for (const SheetData& data : tilemap->getSheets())
				out << data;
			out << YAML::EndSeq;

			out << YAML::Key << "TextureLayers";
			out << YAML::Value << YAML::BeginSeq;
			for (const TileRenderLayer& layer : tilemap->getLayers())
				out << layer;

			out << YAML::EndSeq;

			LAB_SERIALISE_PROPERTY(BehaviourLayer, tilemap->getBehaviour(), out);

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Tilemap

		FileUtils::Write(AssetManager::GetFileSystemPath(metadata), out.c_str());
	}

	bool TilemapSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		fs::path assetPath = AssetManager::GetFileSystemPath(metadata);
		std::string str;
		FileUtils::Read(assetPath, str);
		YAML::Node root = YAML::Load(str);
		YAML::Node mapNode = root["Tilemap"];

		std::string mapName;
		u32 mapWidth, mapHeight;

		LAB_DESERIALISE_PROPERTY(Name, mapName, mapNode);
		LAB_DESERIALISE_PROPERTY(Width, mapWidth, mapNode);
		LAB_DESERIALISE_PROPERTY(Height, mapHeight, mapNode);

		Ref<Tilemap> tilemap = Ref<Tilemap>::Create(mapName, mapWidth, mapHeight);

		auto mapSheets = mapNode["SpriteSheets"];
		for (auto sheet : mapSheets)
		{
			const SheetData& sheetData = sheet.as<SheetData>();
			tilemap->addSheet(sheetData.sheet->handle, sheetData.startIndex);
		}

		auto mapLayers = mapNode["TextureLayers"];
		for (auto texLayer : mapLayers)
		{
			TileRenderLayer renderLayer = texLayer.as<TileRenderLayer>();
			tilemap->addLayer(std::move(renderLayer));
		}

		TileBehaviourLayer behaviourLayer;
		LAB_DESERIALISE_PROPERTY(BehaviourLayer, behaviourLayer, mapNode);
		tilemap->setBehaviour(std::move(behaviourLayer));

		asset = tilemap;
		asset->handle = metadata.handle;

		return true;
	}

	void AnimationSerialiser::serialise(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Animation> animation = asset.to<Animation>();

		YAML::Emitter out;
		out << YAML::BeginMap; // Animation
		out << YAML::Key << "Animation" << YAML::Value;
		{
			out << YAML::BeginMap;

			LAB_SERIALISE_PROPERTY(Name, animation->getName().data(), out);

			out << YAML::Key << "Frames";
			out << YAML::Value << YAML::BeginSeq;
			for (const AnimationFrame& frame : animation->getFrames())
				out << frame;
			out << YAML::EndSeq;

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Animation

		FileUtils::Write(AssetManager::GetFileSystemPath(metadata), out.c_str());
	}

	bool AnimationSerialiser::deserialise(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		fs::path assetPath = AssetManager::GetFileSystemPath(metadata);
		std::string str;
		FileUtils::Read(assetPath, str);
		YAML::Node root = YAML::Load(str);
		YAML::Node animationNode = root["Animation"];

		std::string animatioName;
		LAB_DESERIALISE_PROPERTY(Name, animatioName, animationNode);

		Ref<Animation> animation = Ref<Animation>::Create(animatioName);

		auto frames = animationNode["Frames"];
		for (auto frameNode : frames )
		{
			AnimationFrame frame = frameNode.as<AnimationFrame>();
			animation->addFrame(std::move(frame));
		}

		asset = animation;
		asset->handle = metadata.handle;

		return true;
	}
}