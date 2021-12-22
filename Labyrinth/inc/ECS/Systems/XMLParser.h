#pragma once

#include <string>
#include <vector>
#include <map>

#include "SDL_rect.h"
#include "rapidxml.hpp"

using ColliderList = std::vector<SDL_Rect>;

class XMLParser
{
public:
	static std::map<int, ColliderList> getTileColliders(std::string tileset);

private:
	static rapidxml::xml_node<>* getChild(rapidxml::xml_node<>* inputNode, std::string sNodeFilter);
	static ColliderList getColliders(rapidxml::xml_node<>* inputNode);
};