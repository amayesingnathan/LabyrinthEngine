#pragma once

#include <string>

namespace rapidxml
{
	template<class Ch>
	class xml_node;
}

namespace Labyrinth {

	struct SheetData;
	class TexMapLayer;
	class Tilemap;

	class TiledIO
	{
	public:
		enum class Attribute
		{
			X,
			Y,
			Width,
			Height
		};

	public:
		static void Open(const std::filesystem::path& path, std::vector<TexMapLayer>& layers, std::vector<SheetData>& sheets);

	private:
		static void GetLayers(rapidxml::xml_node<char>* mapNode, std::vector<TexMapLayer>& layers);
		static void GetTilesets(rapidxml::xml_node<char>* mapNode, const std::filesystem::path& lvlPath, std::vector<SheetData>& setData);
		static void GetTileData(const std::filesystem::path& tileset, i32 firstID, std::vector<SheetData>& setData);
		static void GetTileData(rapidxml::xml_node<char>* tilesetNode, i32 firstID, const std::filesystem::path& pngPath, std::vector<SheetData>& setData);

		static rapidxml::xml_node<char>* GetChild(rapidxml::xml_node<char>* inputNode, std::string sNodeFilter);
	};
}