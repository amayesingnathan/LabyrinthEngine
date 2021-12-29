#include "Lpch.h"

#include "ECS\Systems\XMLParser.h"

#include "Scene.h"

using namespace rapidxml;

xml_node<>* XMLParser::getChild(xml_node<>* inputNode, std::string sNodeFilter)
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

void XMLParser::getLayers(xml_node<>* mapNode, std::vector<Layer>& layerList)
{
    int layerCount = 0;
    for (xml_node<>* layer = getChild(mapNode, "layer"); layer; layer = layer->next_sibling())
    {
        layerCount++;
    }

    layerList.reserve(layerCount);

    xml_node<>* layerMetaData = getChild(mapNode, "layer");

    int width = std::atoi(layerMetaData->first_attribute("width")->value());
    int height = std::atoi(layerMetaData->first_attribute("height")->value());

    Layer currLayer;
    std::vector<int> currRow;
    currLayer.reserve(width);
    currRow.reserve(height);

    std::string mapLine;
    std::string mapElement;

    for (xml_node<>* layer = getChild(mapNode, "layer"); layer; layer = layer->next_sibling())
    {
        xml_node<>* layerData = getChild(layer, "data");
        std::istringstream mapStream(layerData->value());

        //Data begins with CRLF so read this first to ignore it.
        std::getline(mapStream, mapLine);

        while (std::getline(mapStream, mapLine))
        {
            std::istringstream lineStream(mapLine);
            while (std::getline(lineStream, mapElement, ','))
            {
                if (mapElement != "\r")
                {
                    currRow.emplace_back(std::stoi(mapElement));
                }
            }
            currLayer.emplace_back(currRow);
            currRow.clear();
        }
        layerList.emplace_back(currLayer);
        currLayer.clear();
    }
}

void XMLParser::getTileData(const std::string& tileset, int firstID, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders)
{
    std::string tilesetTmx = "levels/l";

    //Remove .tsx extension and replace with .png
    std::string tilesetPng = tileset.substr(0, tileset.length() - 4) + ".png";

    //Get initial node
    file<> xmlFile(tileset.c_str());
    xml_document<>* doc = new xml_document<>;
    doc->parse<0>(xmlFile.data());  
    xml_node<>* parent = doc->first_node();

    tilesetData data;

    //Build data struct for this tileset 
    data.tilesetTex = Scene::sysTex.loadTexture(tilesetPng.c_str());
    data.tilesetWidth = std::atoi(parent->first_attribute("columns")->value());
    data.tileWidth = std::atoi(parent->first_attribute("tilewidth")->value());
    data.tileHeight = std::atoi(parent->first_attribute("tileheight")->value());

    setData.emplace(firstID, data);

    //Build list of colliders for each tileID
    for (xml_node<>* tile = getChild(parent, "tile"); tile; tile = tile->next_sibling())
    {
        if (getChild(tile, "objectgroup"))
        {
            //Add key value pair for this tile ID
            int tileID = std::atoi(tile->first_attribute()->value());
            tilesetColliders.emplace(firstID + tileID, getColliders(tile));
        }
    }

    delete doc;
    doc = nullptr;
}

void XMLParser::getTileData(xml_node<>* tilesetNode, int firstID, const std::string& pngPath, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders)
{
    int tileWidth = std::atoi(tilesetNode->first_attribute("tilewidth")->value());
    int tileHeight = std::atoi(tilesetNode->first_attribute("tileheight")->value());
    int tilesetWidth = std::atoi(tilesetNode->first_attribute("columns")->value());
    std::string tilesetPath = pngPath + std::string(getChild(tilesetNode, "image")->first_attribute("source")->value());
    SDL_Texture* tilesetTex = Scene::sysTex.loadTexture(tilesetPath.c_str());

    setData.emplace(firstID, tilesetData{ tilesetTex, tileWidth, tileHeight, tilesetWidth });

    for (xml_node<>* tile = getChild(tilesetNode, "tile"); tile; tile = tile->next_sibling())
    {
        int tileID = std::atoi(tile->first_attribute("id")->value());
        tilesetColliders.emplace(firstID + tileID, getColliders(tile));
    }

}

void XMLParser::getTilesets(xml_node<>* mapNode, const std::string& lvlPath, std::map<int, tilesetData>& setData, std::map<int, ColliderList>& tilesetColliders)
{
    for (xml_node<>* tileset = getChild(mapNode, "tileset"); std::string(tileset->name()) == "tileset"; tileset = tileset->next_sibling())
    {
        int firstGridID = std::atoi(tileset->first_attribute("firstgid")->value());

        //Some tilesets are included inline in file, some stored externally. 
        if (xml_attribute<>* source = tileset->first_attribute("source"))
        {
            std::string tilesetPath = lvlPath + std::string(source->value());
            getTileData(tilesetPath, firstGridID, setData, tilesetColliders);
        }
        else
        {
            getTileData(tileset, firstGridID, lvlPath, setData, tilesetColliders);
        }
    }
}

ColliderList XMLParser::getColliders(rapidxml::xml_node<>* inputNode)
{
    //Initialise vector to return
    ColliderList returnColliders;

    // cycles every collider in group
    for (xml_node<>* collider = getChild(inputNode, "object"); collider; collider = collider->next_sibling())
    {
        Collider currCollider;
        std::string attrName;

        // Cycles every attribute of the collider
        for (xml_attribute<>* nodeAttr = collider->first_attribute("x"); nodeAttr; nodeAttr = nodeAttr->next_attribute())
        {
            attrName = nodeAttr->name();
            double fAttr = std::atof(nodeAttr->value());
            int attr = static_cast<int>(round(fAttr));

            if (attrName == "x")
                currCollider.rect.x = attr;
            if (attrName == "y")
                currCollider.rect.y = attr;
            if (attrName == "width")
                currCollider.rect.w = attr;
            if (attrName == "height")
                currCollider.rect.h = attr;
        }

        if (getChild(collider, "ellipse"))
        {
            currCollider.type = Collider::Type::Trigger;
        }
        else { currCollider.type = Collider::Type::Solid; }

        returnColliders.push_back(currCollider);
    }
    return returnColliders;
}

void XMLParser::openLevel(int lvl, std::vector<Layer>& layerList, std::map<int, tilesetData>& tilesetData, std::map<int, ColliderList>& tilesetColliders)
{
    std::string lvlPath = "levels/level" + std::to_string(lvl) + "/";
    std::string lvlLoc = lvlPath + "level" + std::to_string(lvl) + ".tmx";

    file<> xmlFile(lvlLoc.c_str());
    xml_document<>* doc = new xml_document<>;
    doc->parse<0>(xmlFile.data());

    xml_node<>* mapNode = doc->first_node("map");

    getLayers(mapNode, layerList);
    getTilesets(mapNode, lvlPath, tilesetData, tilesetColliders);

    delete doc;
}