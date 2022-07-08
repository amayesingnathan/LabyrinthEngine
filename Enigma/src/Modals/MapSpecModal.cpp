#include "MapSpecModal.h"

#include "../Panels/ScenePanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    MapSpecModal::MapSpecModal(Entity& data) : mEntity(data) 
    {
    }

    void MapSpecModal::display()
    {
        ImGui::Text("Please enter the specifications for the tilemap:");
        ImGui::NewLine();

        char nameBuf[256];
        memset(nameBuf, 0, sizeof(nameBuf));
        STR_COPY(nameBuf, mMapName);
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
            mMapName = std::string(nameBuf);

        ImGui::NewLine();

        int layer = (int)mLayer;
        ImGui::DragInt("Layer", &layer, 0, 255);
        mLayer = (uint8_t)layer;

        ImGui::NewLine();

        if (ImGui::Button("OK"))
        {
            mEntity.addComponent<TilemapComponent>(mMapName, mLayer);
            Close();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            Close();
    }
}
