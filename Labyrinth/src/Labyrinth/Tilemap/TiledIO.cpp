#include "Lpch.h"
#include "TiledIO.h"

#include "Tilemap.h"

#include "Labyrinth/Assets/AssetManager.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

namespace Labyrinth {

    static const std::unordered_map<std::string, TiledIO::Attribute> AttributeTable =
    {
        {"x", TiledIO::Attribute::X},
        {"y", TiledIO::Attribute::Y},
        {"width", TiledIO::Attribute::Width},
        {"height", TiledIO::Attribute::Height}
    };

	void TiledIO::Open(const std::filesystem::path& path, std::vector<MapLayer>& layers, std::vector<SheetData>& sheets)
	{
        std::filesystem::path lvlName = path.stem();
        std::filesystem::path lvlLoc = path / lvlName;
        lvlLoc += ".tmx";

        rapidxml::file<> xmlFile(lvlLoc.string().c_str());
        rapidxml::xml_document<>* doc = new rapidxml::xml_document<>;
        doc->parse<0>(xmlFile.data());

        rapidxml::xml_node<>* mapNode = doc->first_node("map");

        GetLayers(mapNode, layers);
        GetTilesets(mapNode, path, sheets);

        delete doc;
	}

    void TiledIO::GetLayers(rapidxml::xml_node<>* mapNode, std::vector<MapLayer>& layers)
    {
        {
            int layerCount = 0;
            for (rapidxml::xml_node<>* layer = GetChild(mapNode, "layer"); layer; layer = layer->next_sibling())
                layerCount++;

            layers.reserve(layerCount);
        }

        rapidxml::xml_node<>* layerMetaData = GetChild(mapNode, "layer");

        size_t width = Cast<size_t>(std::stoi(layerMetaData->first_attribute("width")->value()));
        size_t height = Cast<size_t>(std::stoi(layerMetaData->first_attribute("height")->value()));

        std::string mapLine;
        std::string mapElement;

        for (rapidxml::xml_node<>* layer = GetChild(mapNode, "layer"); layer; layer = layer->next_sibling())
        {
            MapLayer currLayer(width, height);
            rapidxml::xml_node<>* layerData = GetChild(layer, "data");
            std::istringstream mapStream(layerData->value());

            //Data begins with CRLF so read this first to ignore it.
            std::getline(mapStream, mapLine);

            while (std::getline(mapStream, mapLine))
            {
                std::istringstream lineStream(mapLine);
                while (std::getline(lineStream, mapElement, ','))
                {
                    if (mapElement != "\r" && mapElement != "\n")
                        currLayer.add(Cast<size_t>(std::stoi(mapElement)));
                }
            }
            layers.emplace_back(std::move(currLayer));
        }
    }

    void TiledIO::GetTilesets(rapidxml::xml_node<>* mapNode, const std::filesystem::path& lvlPath, std::vector<SheetData>& setData)
    {
        for (rapidxml::xml_node<>* tileset = GetChild(mapNode, "tileset"); std::string(tileset->name()) == "tileset"; tileset = tileset->next_sibling())
        {
            size_t firstGridID = Cast<size_t>(std::stoi(tileset->first_attribute("firstgid")->value()));

            //Some tilesets are included inline in file, some stored externally. 
            if (rapidxml::xml_attribute<>* source = tileset->first_attribute("source"))
            {
                std::filesystem::path tilesetPath = lvlPath;
                tilesetPath /= std::filesystem::path(source->value());
                GetTileData(tilesetPath, firstGridID, setData);
            }
            else
                GetTileData(tileset, firstGridID, lvlPath, setData);
        }

        std::sort(setData.begin(), setData.end());
    }

    void TiledIO::GetTileData(const std::filesystem::path& tileset, size_t firstID, std::vector<SheetData>& setData)
    {
        //Remove .tsx extension and replace with .png
        std::filesystem::path tilesetPng = tileset.parent_path();
        tilesetPng += tileset.stem();
        tilesetPng += ".png";

        //Get initial node
        rapidxml::file<> xmlFile(tileset.string().c_str());
        rapidxml::xml_document<>* doc = new rapidxml::xml_document<>;
        doc->parse<0>(xmlFile.data());
        rapidxml::xml_node<>* root = doc->first_node();

        size_t tileWidth = Cast<size_t>(std::stoi(root->first_attribute("tilewidth")->value()));
        size_t tileHeight = Cast<size_t>(std::stoi(root->first_attribute("tileheight")->value()));

        //Build data struct for this tileset 
        setData.emplace_back(firstID, CreateRef<Texture2DSheet>(tilesetPng.string(), glm::vec2{ tileWidth, tileHeight }, std::to_string(firstID)));

        delete doc;
    }

    void TiledIO::GetTileData(rapidxml::xml_node<>* tilesetNode, size_t firstID, const std::filesystem::path& pngPath, std::vector<SheetData>& setData)
    {
        size_t tileWidth = Cast<size_t>(std::stoi(tilesetNode->first_attribute("tilewidth")->value()));
        size_t tileHeight = Cast<size_t>(std::stoi(tilesetNode->first_attribute("tileheight")->value()));
        std::filesystem::path tilesetPath = pngPath;
        tilesetPath /= std::filesystem::path(GetChild(tilesetNode, "image")->first_attribute("source")->value());

        setData.emplace_back(firstID, CreateRef<Texture2DSheet>(tilesetPath.string(), glm::vec2{ tileWidth, tileHeight }, std::to_string(firstID)));
    }

    rapidxml::xml_node<>* TiledIO::GetChild(rapidxml::xml_node<>* inputNode, std::string sNodeFilter)
    {
        // cycles every child
        for (rapidxml::xml_node<>* nodeChild = inputNode->first_node(); nodeChild; nodeChild = nodeChild->next_sibling())
        {
            if (nodeChild->name() == sNodeFilter)
            {
                // returns the desired child
                return nodeChild;
            }
            rapidxml::xml_node<>* x = GetChild(nodeChild, sNodeFilter);
            if (x)
                return x;
        }
        return 0;
    }

}