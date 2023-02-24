#include "Lpch.h"
#include "NewMapModal.h"

#include <glm/gtc/type_ptr.hpp>

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiCpp.h>
#include <Labyrinth/Tilemap/Tilemap.h>

using imcpp::Widgets;

namespace Laby {

    void NewMapModal::onImGuiRender()
    {
        Widgets::Label("Please enter the specifications for the tilemap:");
        Widgets::NewLine();

        Widgets::StringEdit("Name", mMapName);
        Widgets::UIntEdit("Width", mMapWidth);
        Widgets::UIntEdit("Height", mMapHeight);
    }

    void NewMapModal::onCustomButtonRender(bool& open)
    {
        Widgets::Button("Create", [&, this]()
        {
            onComplete();
            open = false;
        });

        Widgets::SameLine();
        
        Widgets::Button("Cancel", [&, this]()
        {
            open = false;
        });
    }

    void NewMapModal::onComplete()
    {
        AssetManager::CreateNewAsset<Tilemap>(mMapName, mMapName, mMapWidth, mMapHeight);
    }
}
