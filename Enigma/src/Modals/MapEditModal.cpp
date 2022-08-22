#include "MapEditModal.h"

#include "../Panels/ScenePanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/IO/Input.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    inline static EditMode EditModeFromString(const std::string& assetType)
    {
        if (assetType == "Behaviour")	return EditMode::Behaviour;
        else if (assetType == "Paint")	return EditMode::Paint;
        else if (assetType == "Brush")	return EditMode::Brush;

        LAB_CORE_ASSERT(false, "Unknown Edit Mode!");
        return EditMode::Behaviour;
    }

    inline static const char* EditModeToString(EditMode assetType)
    {
        switch (assetType)
        {
        case EditMode::Behaviour:	return "Behaviour";
        case EditMode::Paint:		return "Paint";
        case EditMode::Brush:		return "Brush";
        }

        LAB_CORE_ASSERT(false, "Unknown Edit Mode");
        return "None";
    }

    MapEditModal::MapEditModal(const Ref<Tilemap>& map) : EditorModal(), mTilemap(map), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
    }

    void MapEditModal::onImGuiRender()
    {
        bool painting = false;
        if (mEditMode == EditMode::Brush && mHoveredMapTile.valid())
        {
            if (ImGui::IsMouseClicked(Mouse::ButtonLeft, true))
            {
                mTilemap->setTile(mCurrentLayer, mHoveredMapTile, mCurrentTexTile);
                painting = true;
            }
        }

        // Left
        {
            ImGui::BeginChild("Left Pane", ImVec2(300, -3 * ImGui::GetFrameHeightWithSpacing()));

            ImGui::BeginGroup();
            ImGui::BeginChild("Layers", ImVec2(0, -12 * ImGui::GetFrameHeightWithSpacing()), true);
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
            ImGui::SameLine();
            if (ImGui::Button("Move Up"))
            {
                if (mTilemap->moveLayer(mCurrentLayer, LayerDirection::Up))
                {
                    mTilemap->RegenTexture();
                    mCurrentLayer--;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Move Down"))
            {
                if (mTilemap->moveLayer(mCurrentLayer, LayerDirection::Down))
                {
                    mTilemap->RegenTexture();
                    mCurrentLayer++;
                }
            }
            ImGui::EndGroup();

            ImGui::BeginChild("Selected", ImVec2(0, -5 * ImGui::GetFrameHeightWithSpacing()));

            ImGui::Text(fmt::format("Tile: ({}, {})", mCurrentMapTile.x, mCurrentMapTile.y).c_str());

            ImGui::BeginDisabled(mCurrentMapTile.valid());
            const std::string& selectedScript = (mTilemap->getTileBehaviour().count(mCurrentMapTile) != 0) ? mTilemap->getTileBehaviour().at(mCurrentMapTile) : "";
            if (ImGui::BeginCombo("Behaviour", selectedScript.c_str()))
            {
                // Display "None" at the top of the list
                bool clear = selectedScript.empty();
                if (ImGui::Selectable("None", clear))
                    mTilemap->removeTileBehaviour(mCurrentMapTile);

                for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
                {
                    bool isSelected = selectedScript == key;

                    if (ImGui::Selectable(key.c_str(), isSelected))
                        mTilemap->setTileBehaviour(mCurrentMapTile, key);

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }
            ImGui::EndDisabled();

            ImGui::EndChild();

            ImGui::BeginChild("Hovered");
            auto subtexImageSize = ImGui::GetContentRegionAvail();

            ImGui::Text(fmt::format("Hovered Tile: ({}, {})", mHoveredMapTile.x, mHoveredMapTile.y).c_str());
            const std::string& hoveredScript = (mTilemap->getTileBehaviour().count(mHoveredMapTile) != 0) ? mTilemap->getTileBehaviour().at(mHoveredMapTile) : "";
            ImGui::Text(fmt::format("Hovered Behaviour: {}", hoveredScript).c_str());

            subtexImageSize.x *= 0.5f;
            subtexImageSize.y *= 0.5f;

            ImTextureID selectedSubTex = (ImTextureID)(uintptr_t)EditorResources::NoTexture->getRendererID();
            ImVec2 min(0, 1), max(1, 0);

            Ref<SubTexture2D> hoveredSubtex = mTilemap->getTileTex(mHoveredTexTile);
            if (hoveredSubtex && !painting)
            {
                selectedSubTex = (ImTextureID)(uintptr_t)hoveredSubtex->getBaseTex()->getRendererID();
                glm::vec2* coords = hoveredSubtex->getTexCoords();
                max = ImVec2(coords[1].x, coords[1].y);
                min = ImVec2(coords[3].x, coords[3].y);
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.25f * subtexImageSize.x);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.25f * subtexImageSize.y);
            ImGui::Image(selectedSubTex, subtexImageSize, min, max);

            //ImGui::Text(fmt::format("Hovered Tex ID: {}", mHoveredTexTile).c_str());

            ImGui::EndChild();

            ImGui::EndChild();
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

                if (ImGui::Selectable(sheet->getName().c_str(), sheet && mCurrentSheet.sheet == sheet->handle))
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

    void MapEditModal::onEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(MapEditModal::OnKeyPressed));
    }

    void MapEditModal::DrawMap()
    {
        mHoveredMapTile = TilePos();
        mHoveredTexTile = -1;

        f32 xpos = ImGui::GetCursorPosX();
        f32 ypos = ImGui::GetCursorPosY();
        
        auto imageSize = ImGui::GetContentRegionAvail();
        imageSize.y -= ImGui::GetFrameHeightWithSpacing();
        ImGui::Image((ImTextureID)(uintptr_t)mTilemap->getTex()->getRendererID(), imageSize, { 0, 1 }, { 1, 0 });

        f32 startX = ImGui::GetCursorPosX();
        ImGui::Text(fmt::format("Edit Mode: {}", EditModeToString(mEditMode)).c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(startX + (0.2f * imageSize.x));
        ImGui::Text("Ctrl + B: Behaviour Mode");
        ImGui::SameLine();
        ImGui::SetCursorPosX(startX + (0.4f * imageSize.x));
        ImGui::Text("Ctrl + P: Paint Mode");
        ImGui::SameLine();
        ImGui::SetCursorPosX(startX + (0.6f * imageSize.x));
        ImGui::Text("Ctrl + R: Brush Mode");

        ImGuiButtonFlags flags = (mEditMode == EditMode::Paint || mEditMode == EditMode::Behaviour) ? ImGuiButtonFlags_PressedOnClick : ImGuiButtonFlags_None;
        ImVec2 tileSize = { imageSize.x / mMapWidth, imageSize.y / mMapHeight };

        for (size_t y = 0; y < mMapHeight; y++)
        {
            for (size_t x = 0; x < mMapWidth; x++)
            {
                std::string name = fmt::format("##MapTile({}, {})", x, y);
                TilePos pos(x, y);

                ImGui::SetCursorPosX(xpos + (x * tileSize.x));
                ImGui::SetCursorPosY(ypos + (y * tileSize.y));

                if (ImGui::InvisibleButton(name.c_str(), tileSize, flags))
                {
                    if (mEditMode == EditMode::Paint)
                        mTilemap->setTile(mCurrentLayer, pos, mCurrentTexTile);
                    else if (mEditMode == EditMode::Behaviour)
                        mCurrentMapTile = pos;
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MAP_EDIT_TEXTURE_ITEM"))
                    {
                        i32 tileId = *(i32*)payload->Data;
                        mTilemap->setTile(mCurrentLayer, pos, tileId);
                    }
                    ImGui::EndDragDropTarget();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
                {
                    mHoveredMapTile = pos;
                    mHoveredTexTile = mTilemap->getTile(mCurrentLayer, pos);
                }
            }
        }
    }

    void MapEditModal::DrawSheet()
    {
        f32 xpos = ImGui::GetCursorPosX();
        f32 ypos = ImGui::GetCursorPosY();

        auto sheetImageSize = ImGui::GetContentRegionAvail();
        sheetImageSize.y -= 10 * ImGui::GetFrameHeightWithSpacing();

        Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet.sheet);

        {   // Sprite Sheet
            u32 tex = sheet ? sheet->getBaseTex()->getRendererID() : EditorResources::NoTexture->getRendererID();
            ImGui::Image((ImTextureID)(uintptr_t)tex, sheetImageSize, { 0, 1 }, { 1, 0 });
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
                    fs::path sheetPath = Project::GetAssetDirectory() / path;

                    if (AssetManager::IsExtensionValid(sheetPath.extension().string(), AssetType::TextureSheet))
                    {
                        AssetHandle sheetHandle = AssetManager::GetHandleFromPath(sheetPath);
                        if (!mTilemap->hasSheet(sheetHandle))
                        {
                            i32 nextFirstID = 0;
                            if (!mTilemap->getSheets().empty())
                            {
                                const SheetData& lastSheetData = mTilemap->getSheets().back();
                                Ref<Texture2DSheet> lastSheet = AssetManager::GetAsset<Texture2DSheet>(lastSheetData.sheet);
                                nextFirstID = lastSheetData.firstID + (lastSheet->getTileCountX() * lastSheet->getTileCountY());
                            }

                            Ref<Texture2DSheet> newSheet = AssetManager::GetAsset<Texture2DSheet>(sheetHandle);
                            newSheet->clearTileset();
                            newSheet->generateTileset(nextFirstID);
                            mTilemap->addSheet(nextFirstID, newSheet);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }

        {   // Selected Subtexture
            auto subtexImageSize = ImGui::GetContentRegionAvail();
            subtexImageSize.x *= 0.5f;
            subtexImageSize.y -= (3 * ImGui::GetFrameHeightWithSpacing() + 0.25f * subtexImageSize.y);

            ImTextureID selectedSubTex = (ImTextureID)(uintptr_t)EditorResources::NoTexture->getRendererID();
            ImVec2 min(0, 1), max(1, 0);

            if (mCurrentSubTex)
            {
                selectedSubTex = (ImTextureID)(uintptr_t)mCurrentSubTex->getBaseTex()->getRendererID();
                glm::vec2* coords = mCurrentSubTex->getTexCoords();
                max = ImVec2(coords[1].x, coords[1].y);
                min = ImVec2(coords[3].x, coords[3].y);
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.25f * subtexImageSize.x);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.25f * subtexImageSize.y);
            ImGui::Image(selectedSubTex, subtexImageSize, min, max);
            ImGui::SameLine();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.5f * (subtexImageSize.y - ImGui::GetFrameHeightWithSpacing()));
            if (ImGui::Button("Clear Subtex"))
            {
                mCurrentTexTile = -1;
                mCurrentSubTex = nullptr;
            }
        }

        if (!sheet)
            return;

        i32 tileCountX = (i32)sheet->getTileCountX();
        i32 tileCountY = (i32)sheet->getTileCountY();
        LAB_ASSERT(tileCountX > 0 && tileCountY > 0, "Tile sheet count too large!");

        ImVec2 tileSize = { sheetImageSize.x / tileCountX, sheetImageSize.y / tileCountY };
        for (i32 y = 0; y < tileCountY; y++)
        {
            for (i32 x = 0; x < tileCountX; x++)
            {
                std::string name = fmt::format("##SheetTile({}, {})", x, y);
                static i32 tileID = -1;
                tileID = mCurrentSheet.firstID + (x + (y * sheet->getTileCountX()));

                ImGui::SetCursorPosX(xpos + (x * tileSize.x));
                ImGui::SetCursorPosY(ypos + (y * tileSize.y));

                if (ImGui::Button(name.c_str(), tileSize) && mEditMode != EditMode::Behaviour)
                {
                    mCurrentTexTile = tileID;
                    mCurrentSubTex = sheet->getSubTex(tileID);
                }
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload("MAP_EDIT_TEXTURE_ITEM", &tileID, sizeof(i32));
                    ImGui::EndDragDropSource();
                }
            }
        }
    }

    bool MapEditModal::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.isRepeated())
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

        switch (e.getKeyCode())
        {
        case Key::B:
        {
            if (control)
            {
                mEditMode = EditMode::Behaviour;
                return true;
            }
        }
        break;
        case Key::P:
        {
            if (control)
            {
                mEditMode = EditMode::Paint;
                return true;
            }
        }
        break;
        case Key::R:
        {
            if (control)
            {
                mEditMode = EditMode::Brush;
                return true;
            }
        }
        break;
        }

        return false;
    }
}
