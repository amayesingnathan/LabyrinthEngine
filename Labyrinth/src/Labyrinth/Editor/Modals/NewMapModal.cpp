#include "Lpch.h"
#include "NewMapModal.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/Tilemap/Tilemap.h>

namespace Laby {

    void NewMapModal::onImGuiRender()
    {
        ImGui::Text("Please enter the specifications for the tilemap:");
        ImGui::NewLine();

        StaticString<256> nameBuf(mMapName);
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
            mMapName = nameBuf.toString();

        i32 width = (i32)mMapWidth;
        ImGui::InputInt("Width", &width);
        if (width < 0)
            width = 0;
        mMapWidth = (usize)width;

        i32 height = (i32)mMapHeight;
        ImGui::InputInt("Height", &height);
        if (height < 0)
            height = 0;
        mMapHeight = (usize)height;
    }

    void NewMapModal::onCustomButtonRender(bool& open)
    {
        if (ImGui::Button("Create"))
        {
            onComplete();
            open = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            open = false;
    }

    void NewMapModal::onComplete()
    {
        AssetManager::CreateNewAsset<Tilemap>(fmt::format("{}.lmap", mMapName), "tilemaps", mMapName, mMapWidth, mMapHeight);
    }
}
