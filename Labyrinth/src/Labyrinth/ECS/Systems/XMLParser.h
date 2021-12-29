#pragma once

#if 0

#include "ECS/Components/Vector2D.h"

//XML Parsing Includes
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

struct Collider
{
	//THIS MUST MIRROR ENUM "Type" IN ColliderComponent.h
	enum class Type { Solid = 0, Trigger };

	SDL_Rect rect;
	Type type;
};

struct tilesetData
{
	SDL_Texture* tilesetTex;
	int tileWidth;
	int tileHeight;
	int tilesetWidth;
};

using ColliderList = std::vector<Collider>;
using Layer = std::vector<std::vector<int>>;

class XMLParser
{
public:
	static void openLevel(int lvl, std::vector<Layer>& layerList, std::map<int, tilesetData>& tilesets, std::map<int, ColliderList>& tilesetColliders);

private:
	static void getLayers(rapidxml::xml_node<>* mapNode, std::vector<Layer>& layerList);
	static void getTilesets(rapidxml::xml_node<>* mapNode, const std::string& lvlPath, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders);
	static void getTileData(const std::string& tileset, int firstID, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders);
	static void getTileData(rapidxml::xml_node<>* tilesetNode, int firstID, const std::string& pngPath, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders);
		
	static rapidxml::xml_node<>* getChild(rapidxml::xml_node<>* inputNode, std::string sNodeFilter);
	static ColliderList getColliders(rapidxml::xml_node<>* inputNode);
};

#endif