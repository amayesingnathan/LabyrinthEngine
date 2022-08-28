#include "TilemapPanel.h"

#include "../Modals/MapEditModal.h" 

#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Tilemap/Tilemap.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	void TilemapPanel::onImGuiRender()
	{
        ImGui::Begin("Tilemaps");

        const auto& tilemaps = AssetManager::GetAssetsWithType(AssetType::Tilemap);
        if (ImGui::BeginListBox("##Tilemaps", ImVec2(-FLT_MIN, -FLT_MIN)))
        {
            for (const AssetHandle& handle : tilemaps)
            {
                const bool isSelected = (mSelected == handle);

                std::string tilemapLabel("Error retrieving tilemap asset...");
                Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(handle);
                if (tilemap)
                    tilemapLabel = fmt::format("Name: {0}\tWidth: {1}\tHeight: {2}", tilemap->getName(), tilemap->getWidth(), tilemap->getHeight());

                if (ImGui::Selectable(tilemapLabel.c_str(), isSelected))
                    mSelected = handle;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                        AssetManager::DestroyAsset(handle);

                    ImGui::EndPopup();
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    ModalManager::Open<MapEditModal>("Edit Tilemap...", ModalButtons::OKCancel, tilemap);

                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("TILEMAP_ITEM", &handle, sizeof(AssetHandle));
                    ImGui::EndDragDropSource();
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        ImGui::End();
	}
}