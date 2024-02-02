#include "Lpch.h"
#include "NewMapModal.h"

#include <glm/gtc/type_ptr.hpp>

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Tilemap/Tilemap.h>

namespace Laby {

    void NewMapModal::OnRender()
    {
        Widgets::Label("Please enter the specifications for the tilemap:");
        Widgets::NewLine();

        Widgets::StringEdit("Name", mMapName);
        Widgets::UIntEdit("Width", mMapWidth);
        Widgets::UIntEdit("Height", mMapHeight);
    }

    void NewMapModal::OnCustomButtonRender(bool& open)
    {
        Widgets::Button("Create", [&, this]()
        {
            OnComplete();
            open = false;
        });

        Widgets::SameLine();
        
        Widgets::Button("Cancel", [&, this]()
        {
            open = false;
        });
    }

    void NewMapModal::OnComplete()
    {
        AssetManager::CreateNewAsset<Tilemap>(mMapName, mMapName, mMapWidth, mMapHeight);
    }
}
