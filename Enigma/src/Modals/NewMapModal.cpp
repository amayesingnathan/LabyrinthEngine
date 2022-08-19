#include "NewMapModal.h"

#include "../Panels/ScenePanel.h"

#include <Labyrinth/Assets/AssetManager.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    void NewMapModal::onImGuiRender()
    {
        ImGui::Text("Please enter the specifications for the tilemap:");
        ImGui::NewLine();

        StaticBuffer<256> nameBuf(mMapName);
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
            mMapName = nameBuf.string();

        ImGui::InputInt("Width", &mMapWidth);
        ImGui::InputInt("Height", &mMapHeight);
    }

    void NewMapModal::onCustomButtonRender()
    {
        if (ImGui::Button("Create"))
        {
            mMapPath = fs::path();
            onComplete();
            onClose();
        }

        ImGui::SameLine();

        if (ImGui::Button("Import"))
        {
            fs::path result = FileUtils::OpenFile({ "Labyrinth Project (*.lpj)", "*.lpj" });
            if (!result.empty())
            {
                mMapPath = fs::relative(result);
                onComplete();
                onClose();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            onClose();
    }

    void NewMapModal::onComplete()
    {
        if (mMapPath.empty())
            AssetManager::CreateNewAsset<Tilemap>(fmt::format("{}.lmap", mMapName), "tilemaps", mMapName, mMapWidth, mMapHeight);
        else
            AssetManager::CreateNewAsset<Tilemap>(fmt::format("{}.lmap", mMapPath.stem().string()), "tilemaps", mMapPath);
    }
}
