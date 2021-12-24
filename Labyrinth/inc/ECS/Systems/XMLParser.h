#pragma once

#include "ECS/Components/Vector2D.h"

#include <string>
#include <vector>
#include <map>

#include "SDL.h"
#include "rapidxml.hpp"

using ColliderList = std::vector<SDL_Rect>;
using Layer = std::vector<std::vector<int>>;

struct tilesetData
{
	SDL_Texture* tilesetTex;
	int tileWidth;
	int tileHeight;
	int tilesetWidth;
};

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