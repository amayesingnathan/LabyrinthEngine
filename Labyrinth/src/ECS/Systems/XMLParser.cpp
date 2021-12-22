#include "ECS\Systems\XMLParser.h"

#include "rapidxml_utils.hpp"

using namespace rapidxml;

std::map<int, ColliderList> XMLParser::getTileData(std::string tileset, int& width)
{
    std::map<int, ColliderList> tilesetColliders;

    //Open and parse data for supplied tileset
    tileset = "assets/textures/" + tileset + ".tsx";
    file<> xmlFile(tileset.c_str());
    xml_document<>* doc = new xml_document<>;   
    doc->parse<0>(xmlFile.data());             

    //Get initial node
    xml_node<>* parent = doc->first_node();

    width = std::atoi(parent->first_attribute("columns")->value());

    //Build list of colliders for each tileID
    for (xml_node<>* tile = getChild(parent, "tile"); tile; tile = tile->next_sibling())
    {
        //Add key value pair for this tile ID
        int key = std::atoi(tile->first_attribute()->value());
        tilesetColliders.emplace(key, getColliders(tile));
    }

    delete doc;
    doc = nullptr;

    return tilesetColliders;
}

rapidxml::xml_node<>* XMLParser::getChild(rapidxml::xml_node<>* inputNode, std::string sNodeFilter)
{
    // cycles every child
    for (xml_node<>* nodeChild = inputNode->first_node(); nodeChild; nodeChild = nodeChild->next_sibling())
    {
        if (nodeChild->name() == sNodeFilter)
        {
            // returns the desired child
            return nodeChild;
        }
        xml_node<>* x = getChild(nodeChild, sNodeFilter);
        if (x)
            return x;
    }
    return 0;
}

ColliderList XMLParser::getColliders(rapidxml::xml_node<>* inputNode)
{
    //Initialise vector to return
    ColliderList returnColliders;

    //Gets the object group node
    xml_node<>* colliderGroup = inputNode->first_node();

    // cycles every collider in group
    for (xml_node<>* collider = colliderGroup->first_node(); collider; collider = collider->next_sibling())
    {
        SDL_Rect currCollider;
        std::string attrName;

        // Cycles every attribute of the collider
        for (xml_attribute<>* nodeAttr = collider->first_attribute("x"); nodeAttr; nodeAttr = nodeAttr->next_attribute())
        {
            attrName = nodeAttr->name();
            double fAttr = std::atof(nodeAttr->value());
            int attr = static_cast<int>(round(fAttr));

            if (attrName == "x")
                currCollider.x = attr;
            if (attrName == "y")
                currCollider.y = attr;
            if (attrName == "width")
                currCollider.w = attr;
            if (attrName == "height")
                currCollider.h = attr;
        }
        returnColliders.push_back(currCollider);
    }
    return returnColliders;
}
