#include "MapEditModal.h"

#include "../Panels/ScenePanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/Editor/EditorResources.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    MapEditModal::MapEditModal(const Ref<Tilemap>& map) : EditorModal(), mTilemap(map), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
    }

    void MapEditModal::onImGuiRender()
    {
        // Left
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("Layers", ImVec2(150, -3 * ImGui::GetFrameHeightWithSpacing()), true);
            for (const TexMapLayer& layer : mTilemap->getLayers())
            {
                usize layerIndex = layer.getLayer();
                std::string layerLit = fmt::format("Layer {}", layerIndex);

                if (ImGui::Selectable(layerLit.c_str(), mCurrentLayer == layerIndex))
                    mCurrentLayer = layerIndex;
            }
            ImGui::EndChild();
            if (ImGui::Button("Add"))
                mTilemap->addLayer();
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                mTilemap->removeLayer(mCurrentLayer);
                mTilemap->RegenTexture();
            }
            ImGui::EndGroup();
        }

        ImGui::SameLine();

        auto& colours = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
        const auto& buttonActive = colours[ImGuiCol_ButtonActive];

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

        // Centre
        {
            ImGui::BeginChild("Map", ImVec2(-400, -3 * ImGui::GetFrameHeightWithSpacing())); 
            DrawMap();
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Centre
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("Sheets", ImVec2(0, 200));
            for (const SheetData& sheetData : mTilemap->getSheets())
            {
                Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(sheetData.sheet);

                if (ImGui::Selectable(sheet->getName().c_str(), sheet && mCurrentSheet.sheet== sheet->handle))
                    mCurrentSheet = SheetData(sheet->getSubTexList().begin()->first, sheet->handle);
            }
            ImGui::EndChild();

            DrawSheet();

            ImGui::EndGroup();
        }

        ImGui::PopStyleColor(3);
    }

    void MapEditModal::onComplete()
    {
        AssetImporter::Serialise(mTilemap);
    }

    void MapEditModal::DrawMap()
    {
        float xpos = ImGui::GetCursorPosX();
        float ypos = ImGui::GetCursorPosY();

        auto imageSize = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(uintptr_t)mTilemap->getTex()->getRendererID(), imageSize);

        ImVec2 tileSize = { imageSize.x / mMapWidth, imageSize.y / mMapHeight };
        for (size_t y = 0; y < mMapHeight; y++)
        {
            for (size_t x = 0; x < mMapWidth; x++)
            {
                std::string name = fmt::format("##MapTile({}, {})", x, y);

                ImGui::SetCursorPosX(xpos + (x * tileSize.x));
                ImGui::SetCursorPosY(ypos + (y * tileSize.y));

                if (ImGui::Button(name.c_str(), tileSize) && SelectionManager::GetSelectionCount(SelectionDomain::Tilemap) != 0)
                {
                    usize tileId = SelectionManager::GetSelection(SelectionDomain::Tilemap, 0);
                    mTilemap->setTile(mCurrentLayer, x, y, (i32)tileId);
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MAP_EDIT_TEXTURE_ITEM"))
                    {
                        i32 tileId = *(i32*)payload->Data;
                        mTilemap->setTile(mCurrentLayer, x, y, tileId);
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
    }

    void MapEditModal::DrawSheet()
    {
        float xpos = ImGui::GetCursorPosX();
        float ypos = ImGui::GetCursorPosY();

        auto imageSize = ImGui::GetContentRegionAvail();
        imageSize.y -= 3 * ImGui::GetFrameHeightWithSpacing();

        Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet.sheet);

        u32 tex = sheet ? sheet->getBaseTex()->getRendererID() : EditorResources::NoTexture->getRendererID();
        ImGui::Image((ImTextureID)(uintptr_t)tex, imageSize, { 0, 1 }, { 1, 0 });
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
                fs::path sheetPath = Project::GetAssetDirectory() / path;

                if (AssetManager::IsExtensionValid(sheetPath.extension().string(), AssetType::TextureSheet))
                {
                    i32 nextFirstID = 0;
                    if (!mTilemap->getSheets().empty())
                    {
                        const SheetData& lastSheetData = mTilemap->getSheets().back();
                        Ref<Texture2DSheet> lastSheet = AssetManager::GetAsset<Texture2DSheet>(lastSheetData.sheet);
                        nextFirstID = lastSheetData.firstID + (lastSheet->getTileCountX() * lastSheet->getTileCountY());
                    }

                    Ref<Texture2DSheet> newSheet = AssetManager::GetAsset<Texture2DSheet>(sheetPath);
                    newSheet->clearTileset();
                    newSheet->generateTileset(nextFirstID);
                    mTilemap->addSheet(nextFirstID, newSheet);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (!sheet)
            return;

        ImVec2 tileSize = { imageSize.x / sheet->getTileCountX(), imageSize.y / sheet->getTileCountY() };
        for (usize y = 0; y < sheet->getTileCountY(); y++)
        {
            for (usize x = 0; x < sheet->getTileCountX(); x++)
            {
                std::string name = fmt::format("##SheetTile({}, {})", x, y);
                static i32 tileID = -1;
                tileID = mCurrentSheet.firstID + (x + (y * sheet->getTileCountX()));

                ImGui::SetCursorPosX(xpos + (x * tileSize.x));
                ImGui::SetCursorPosY(ypos + (y * tileSize.y));

                if (ImGui::Button(name.c_str(), tileSize) && SelectionManager::GetSelection(SelectionDomain::Tilemap, 0))
                {
                    SelectionManager::DeselectAll(SelectionDomain::Tilemap);
                    SelectionManager::Select(SelectionDomain::Tilemap, tileID);
                }
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload("MAP_EDIT_TEXTURE_ITEM", &tileID, sizeof(i32));
                    ImGui::EndDragDropSource();
                }
            }
        }
    }
}
