#include "MapEditModal.h"

#include "../Panels/ScenePanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/ImGui/ImGuiUtils.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/RenderCommand.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    inline static EditMode EditModeFromString(const std::string& assetType)
    {
        if (assetType == "Behaviour")	return EditMode::Behaviour;
        else if (assetType == "Paint")	return EditMode::Paint;

        LAB_CORE_ASSERT(false, "Unknown Edit Mode!");
        return EditMode::Behaviour;
    }

    inline static const char* EditModeToString(EditMode assetType)
    {
        switch (assetType)
        {
        case EditMode::Behaviour:	return "Behaviour";
        case EditMode::Paint:		return "Paint";
        }

        LAB_CORE_ASSERT(false, "Unknown Edit Mode");
        return "None";
    }

    MapEditModal::MapEditModal(const Ref<Tilemap>& map) : EditorModal(), mTilemap(Tilemap::Clone(map)), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
        mTilemap->RegenTexture(mDisplayColliders);

        FramebufferSpec fbSpec;
        fbSpec.width = 256;
        fbSpec.height = 256;
        fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.samples = 1;

        mCurrentSubTexFBO = Framebuffer::Create(fbSpec);
        mHoveredSubTexFBO = Framebuffer::Create(fbSpec);
    }

    void MapEditModal::onUpdate(Timestep ts)
    {
        if (mCurrentSubTex)
        {
            // Draw texture/subtexture to framebuffer
            mCurrentSubTexFBO->bind();

            RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
            RenderCommand::Clear();

            OrthographicCamera camera(-128.0, 128.0, -128.0, 128.0);
            Renderer2D::BeginState(camera);
            Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 256.0f, 256.0f }, (f32)mCurrentTexTile.rotation, mCurrentSubTex);
            Renderer2D::EndState();
            mCurrentSubTexFBO->unbind();
        }

        Ref<SubTexture2D> hoveredSubtex = mTilemap->getTileTex(mHoveredTexTile.id);
        if (hoveredSubtex)
        {
            // Draw texture/subtexture to framebuffer
            mHoveredSubTexFBO->bind();

            RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
            RenderCommand::Clear();

            OrthographicCamera camera(-128.0, 128.0, -128.0, 128.0);
            Renderer2D::BeginState(camera);
            Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 256.0f, 256.0f }, (f32)mHoveredTexTile.rotation, hoveredSubtex);
            Renderer2D::EndState();
            mHoveredSubTexFBO->unbind();
        }
    }

    void MapEditModal::onImGuiRender()
    {
        if (mEditMode == EditMode::Paint && mHoveredMapTile.valid() && mHoveredTexTile != mCurrentTexTile)
        {
            if (ImGui::IsMouseDown(Mouse::ButtonLeft))
            {
                mTilemap->setTile(mCurrentLayer, mHoveredMapTile, mCurrentTexTile);
                mTilemap->RegenTexture(mDisplayColliders);
                mPainting = true;
            }
        }
        if (mPainting && ImGui::IsMouseReleased(Mouse::ButtonLeft))
            mPainting = false;

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
                mTilemap->RegenTexture(mDisplayColliders);
            }
            ImGui::SameLine();
            if (ImGui::Button("Move Up"))
            {
                if (mTilemap->moveLayer(mCurrentLayer, LayerDirection::Up))
                {
                    mTilemap->RegenTexture(mDisplayColliders);
                    mCurrentLayer--;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Move Down"))
            {
                if (mTilemap->moveLayer(mCurrentLayer, LayerDirection::Down))
                {
                    mTilemap->RegenTexture(mDisplayColliders);
                    mCurrentLayer++;
                }
            }
            ImGui::EndGroup();

            {   // Selected Tile
                ImGui::BeginChild("Selected", ImVec2(0, -7 * ImGui::GetFrameHeightWithSpacing()));

                ImGui::Text(fmt::format("Tile: ({}, {})", mCurrentMapTile.x, mCurrentMapTile.y).c_str());

                bool selectedHasData = mTilemap->getTileData().count(mCurrentMapTile) != 0;

                ImGui::BeginDisabled(!mCurrentMapTile.valid());
                const std::string& selectedScript = selectedHasData ? mTilemap->getTileData().at(mCurrentMapTile).script : "";
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

                bool selectedSolid = selectedHasData ? mTilemap->getTileData().at(mCurrentMapTile).solid : false;
                if (ImGui::Checkbox("Solid", &selectedSolid))
                    mTilemap->setTileSolid(mCurrentMapTile, selectedSolid);


                ImGui::EndDisabled();

                ImGui::EndChild();
            }

            {   // Hovered Tile
                ImGui::BeginChild("Hovered");
                auto subtexImageSize = ImGui::GetContentRegionAvail();

                bool hoveredHasData = mTilemap->getTileData().count(mHoveredMapTile) != 0;

                ImGui::BeginDisabled();
                ImGui::Text(fmt::format("Hovered Tile: ({}, {})", mHoveredMapTile.x, mHoveredMapTile.y).c_str());

                const std::string& hoveredScript = hoveredHasData ? mTilemap->getTileData().at(mHoveredMapTile).script : "";
                ImGui::Text(fmt::format("Hovered Behaviour: {}", hoveredScript).c_str());

                bool hoveredSolid = hoveredHasData ? mTilemap->getTileData().at(mHoveredMapTile).solid : false;
                ImGui::Checkbox("Solid", &hoveredSolid);
                ImGui::EndDisabled();

                subtexImageSize.x *= 0.5f;
                subtexImageSize.y *= 0.5f;

                ImTextureID selectedSubTex = (ImTextureID)(uintptr_t)EditorResources::NoTexture->getRendererID();

                Ref<SubTexture2D> hoveredSubtex = mTilemap->getTileTex(mHoveredTexTile.id);
                if (hoveredSubtex && !mPainting)
                    selectedSubTex = (ImTextureID)(uintptr_t)mHoveredSubTexFBO->getColourAttachmentRendererID();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.25f * subtexImageSize.x);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.25f * subtexImageSize.y);
                ImGui::Image(selectedSubTex, subtexImageSize, ImVec2(0, 1), ImVec2(1, 0));

                ImGui::EndChild();
            }

            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Centre
        {
            ImGui::BeginChild("Map", ImVec2(-400, -3 * ImGui::GetFrameHeightWithSpacing())); 
            DrawMap();
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Right
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
    }

    void MapEditModal::onComplete()
    {
        AssetImporter::Serialise(mTilemap);
        AssetManager::ReloadData(mTilemap->handle);
    }

    void MapEditModal::onEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(MapEditModal::OnKeyPressed));
    }

    void MapEditModal::DrawMap()
    {
        mHoveredMapTile = TilePos();
        mHoveredTexTile = TileData();

        ImVec2 pos = ImGui::GetCursorPos();
        
        auto imageSize = ImGui::GetContentRegionAvail();
        imageSize.y -= ImGui::GetFrameHeightWithSpacing();
        ImGui::Image((ImTextureID)(uintptr_t)mTilemap->getTex()->getColourAttachmentRendererID(), imageSize, { 0, 1 }, { 1, 0 });

        ImGui::Text(fmt::format("Edit Mode: {}", EditModeToString(mEditMode)).c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(pos.x + (0.2f * imageSize.x));
        ImGui::Text("Ctrl + B: Behaviour Mode");
        ImGui::SameLine();
        ImGui::SetCursorPosX(pos.x + (0.4f * imageSize.x));
        ImGui::Text("Ctrl + P: Paint Mode");
        ImGui::SameLine();
        ImGui::SetCursorPosX(pos.x + (0.75f * imageSize.x));
        if (ImGui::Checkbox("Colliders", &mDisplayColliders))
            mTilemap->RegenTexture(mDisplayColliders);

        auto& colours = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
        const auto& buttonActive = colours[ImGuiCol_ButtonActive];

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, EditorResources::ClearColour);

        EditorUI::GridControl(pos, imageSize, mMapWidth, mMapHeight, [this](const TilePos& pos, ImVec2 elementSize)
        {
            std::string name = fmt::format("##MapTiles({}, {})", pos.x, pos.y);

            if (ImGui::Button(name.c_str(), elementSize) && mEditMode == EditMode::Behaviour)
                mCurrentMapTile = pos;

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MAP_EDIT_TEXTURE_ITEM"))
                {
                    TileData tileData = *(TileData*)payload->Data;
                    mTilemap->setTile(mCurrentLayer, pos, tileData);
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
            {
                mHoveredMapTile = pos;
                mHoveredTexTile = mTilemap->getTile(mCurrentLayer, pos);
            }
        });

        ImGui::PopStyleColor(3);
    }

    void MapEditModal::DrawSheet()
    {
        ImVec2 firstPos = ImGui::GetCursorPos();

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

            if (mCurrentSubTex)
                selectedSubTex = (ImTextureID)(uintptr_t)mCurrentSubTexFBO->getColourAttachmentRendererID();

            ImVec2 pos = ImGui::GetCursorPos();

            ImGui::SetCursorPosX(pos.x + 0.25f * subtexImageSize.x);
            ImGui::SetCursorPosY(pos.y + 0.25f * subtexImageSize.y);
            //EditorUI::RotatedImage(selectedSubTex, subtexImageSize, mCurrentTexTile.rotation, min, max);
            ImGui::Image(selectedSubTex, subtexImageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::SameLine();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (subtexImageSize.y - 4 * ImGui::GetFrameHeightWithSpacing()));

            pos = ImGui::GetCursorPos();
            if (ImGui::Button("Clear Subtex"))
            {
                mCurrentTexTile = TileData();
                mCurrentSubTex = nullptr;
            }
            pos.y += ImGui::GetFrameHeightWithSpacing();
            ImGui::SetCursorPos(pos);
            if (ImGui::Button("Rotate Right"))
                mCurrentTexTile.rotation -= 90;
            pos.y += ImGui::GetFrameHeightWithSpacing();
            ImGui::SetCursorPos(pos);
            if (ImGui::Button("Rotate Left"))
                mCurrentTexTile.rotation += 90;
        }

        if (!sheet)
            return;

        i32 tileCountX = (i32)sheet->getTileCountX();
        i32 tileCountY = (i32)sheet->getTileCountY();
        LAB_ASSERT(tileCountX > 0 && tileCountY > 0, "Tile sheet count too large!");

        auto& colours = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
        const auto& buttonActive = colours[ImGuiCol_ButtonActive];

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, EditorResources::ClearColour);

        EditorUI::GridControl(firstPos, sheetImageSize, tileCountX, tileCountY, [this, &sheet](const TilePos& pos, const ImVec2& elementSize) 
        {
            static TileData TileData; // Must be static for lifetime to persist for use as payload.

            TileData.id = mCurrentSheet.firstID + (pos.x + (pos.y * sheet->getTileCountX()));
            std::string name = fmt::format("##SheetTile({}, {})", pos.x, pos.y);
            if (ImGui::Button(name.c_str(), elementSize) && mEditMode == EditMode::Paint)
            {
                mCurrentTexTile = TileData;
                mCurrentSubTex = sheet->getSubTex(TileData.id);
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::SetDragDropPayload("MAP_EDIT_TEXTURE_ITEM", &TileData, sizeof(TileData));
                ImGui::EndDragDropSource();
            }
        });

        ImGui::PopStyleColor(3);
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
        }

        return false;
    }
}
