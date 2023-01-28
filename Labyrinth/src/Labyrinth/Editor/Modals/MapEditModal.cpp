#include "Lpch.h"
#include "MapEditModal.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Tools/EnumUtils.h>

namespace Laby {

    using SheetEntry = ComboEntry<AssetHandle>;
    using ScriptClassEntry = ComboEntry<Ref<ScriptClass>>;

    static bool HasSheetEntry(AssetHandle sheet, const std::vector<SheetEntry>& entries)
    {
        return std::find_if(entries.begin(), entries.end(), [sheet](const SheetEntry& entry) { return entry.value == sheet; }) != entries.end();
    }

    static TileID GetStartIndex(Ref<Texture2DSheet> sheet, const std::vector<SheetData>& sheets)
    {
        auto it = std::find_if(sheets.begin(), sheets.end(), [&](const SheetData& data) { return data.sheet == sheet; });
        if (it == sheets.end())
            return NullTileID;

        return it->startIndex;
    }

    MapEditModal::MapEditModal(const Ref<Tilemap>& map)
        : mTilemap(TilemapUtils::Clone(map)), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
    }

    void MapEditModal::onImGuiRender()
    {
        if (mEditMode == EditMode::Paint && ImGuiUtils::IsMouseDown(Mouse::ButtonLeft))
        {
            mCurrentlyPainting = true;
            if (mHoveredTile.valid() && mHoveredSubtex != mBrushSubtex)
                mTilemap->setTileData(mCurrentLayer, mHoveredTile, mBrushSubtex);
        }
        if (mCurrentlyPainting && ImGuiUtils::IsMouseReleased(Mouse::ButtonLeft))
            mCurrentlyPainting = false;

        mFrameHeightWithSpacing = ImGuiUtils::FrameHeightWithSpacing();

        LeftPane();
        Widgets::SameLine();
        CentrePane();
        Widgets::SameLine();
        RightPane();
    }

    void MapEditModal::onComplete()
    {
        AssetImporter::Serialise(mTilemap);
        AssetManager::ReloadData(mTilemap->handle);
    }

    void MapEditModal::onEvent(Event& e)
    {
        LocalEventDispatcher dispatcher(e);
        dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(MapEditModal::OnKeyPressed));
    }

    void MapEditModal::LeftPane()
    {
        Widgets::BeginChild("Left Pane", { 300, -3 * mFrameHeightWithSpacing }, false);

        Widgets::BeginGroup();

        Widgets::BeginChild("Layers", { 0, -12 * mFrameHeightWithSpacing });
        for (const auto& layer : mTilemap->getLayers())
        {
            usize layerIndex = layer.getLayer();
            std::string layerLit = fmt::format("Layer {}", layerIndex);
            Widgets::Selectable(layerLit.c_str(), mCurrentLayer == layerIndex, [&]() { mCurrentLayer = layerIndex; });
        }
        Widgets::EndChild(); // Layers

        Widgets::Button("Add", [this]() { mTilemap->addLayer(); });
        Widgets::SameLine();
        Widgets::Button("Remove", [this]() { mTilemap->removeLayer(mCurrentLayer); });
        Widgets::SameLine();
        Widgets::Button("Move Up", [this]() { mTilemap->moveLayer(mCurrentLayer, LayerMoveDir::Up); });
        Widgets::SameLine();
        Widgets::Button("Move Down", [this]() { mTilemap->moveLayer(mCurrentLayer, LayerMoveDir::Down); });

        Widgets::EndGroup();


        Widgets::BeginChild("Selected", { 0, -7 * mFrameHeightWithSpacing });
        Widgets::Label(fmt::format("Tile: ({}, {})", mCurrentTile.x, mCurrentTile.y));

        bool validCurrentTile = mCurrentTile.valid();
        TileBehaviourData currentTileData = mTilemap->getTileBehaviour(mCurrentTile);

        std::vector<ScriptClassEntry> comboEntries;
        comboEntries.emplace_back("", nullptr);
        for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
            comboEntries.emplace_back(key, klass);

        Widgets::Disable(validCurrentTile);

        Ref<ScriptClass> scriptClass = ScriptEngine::GetAppClass(currentTileData.script);
        Widgets::Combobox("Behaviour", currentTileData.script, scriptClass, comboEntries.data(), comboEntries.size(),
            [&](std::string_view name, Ref<ScriptClass> klass) { currentTileData.script = name; });

        Widgets::Checkbox("Solid", currentTileData.solid);
        if (validCurrentTile)
            mTilemap->setTileBehaviour(mCurrentTile, currentTileData);

        Widgets::EndDisable();

        Widgets::EndChild(); // Selected


        Widgets::BeginChild("Hovered");

        Widgets::Disable();

        glm::vec2 imageSize = 0.5f * ImGuiUtils::AvailableRegion();
        Widgets::Label(fmt::format("Hovered Tile: ({}, {})", mHoveredTile.x, mHoveredTile.y));
        Widgets::Label(fmt::format("Script: {}", mHoveredBehaviour.script));
        Widgets::Checkbox("Solid", mHoveredBehaviour.solid);

        Widgets::EndDisable();

        Ref<IRenderable> image = EditorResources::NoTexture;
        f32 rotation = 0.0f;
        if (mHoveredTile.valid())
        {
            image = mTilemap->getTileTex(mHoveredSubtex.textureID);
            rotation = mHoveredSubtex.rotation;
        }
        if (mCurrentlyPainting)
        {
            image = mTilemap->getTileTex(mBrushSubtex.textureID);
            rotation = mBrushSubtex.rotation;
        }

        ImGuiUtils::SetCursorPos(ImGuiUtils::CursorPos() + 0.25f * imageSize);
        Widgets::Image(image, imageSize, rotation);

        Widgets::EndChild(); // Hovered

        Widgets::EndChild(); // Left Pane
    }

    void MapEditModal::CentrePane()
    {
        Widgets::BeginChild("Map", { -400, -3 * mFrameHeightWithSpacing });

        const glm::vec2& pos = ImGuiUtils::CursorPos();

        auto imageSize = ImGuiUtils::AvailableRegion();
        imageSize.y -= mFrameHeightWithSpacing;
        Widgets::Image(mTilemap, imageSize);

        Widgets::Label(fmt::format("Edit Mode: {}", Enum::ToString(mEditMode)));
        Widgets::SameLine();
        ImGuiUtils::SetCursorPosX(pos.x + (0.2f * imageSize.x));
        Widgets::Label("Ctrl + B: Behaviour Mode");
        Widgets::SameLine();
        ImGuiUtils::SetCursorPosX(pos.x + (0.4f * imageSize.x));
        Widgets::Label("Ctrl + P: Paint Mode");
        Widgets::SameLine();
        ImGuiUtils::SetCursorPosX(pos.x + (0.75f * imageSize.x));
        Widgets::Checkbox("Colliders", mDisplayColliders);

        ImGuiUtils::SetButtonTransparent();
        Widgets::GridControl(pos, imageSize, mMapWidth, mMapHeight, [this](const GridPosition& pos, const glm::vec2 elementSize)
        {
            std::string name = fmt::format("##MapTiles({}, {})", pos.x, pos.y);
            Widgets::Button(name.c_str(), elementSize, [&]() { mCurrentTile = pos; });
            Widgets::OnWidgetHovered([&]()
            {
                mHoveredTile = pos;
                mHoveredSubtex = mTilemap->getTileData(mCurrentLayer, pos);
                mHoveredBehaviour = mTilemap->getTileBehaviour(pos);
            });
            Widgets::OnWidgetSelected([&]()
            {
                mCurrentTile = mHoveredTile;
                mCurrentSubtex = mHoveredSubtex;
                mCurrentBehaviour = mHoveredBehaviour;
            });
        });
        ImGuiUtils::ResetButtonTransparency();

        Widgets::EndChild();
    }

    void MapEditModal::RightPane()
    {
        Widgets::BeginGroup();

        {   // Sheets on Tilemap
            std::vector<SheetEntry> tilemapSheets;
            for (const SheetData& sheetData : mTilemap->getSheets())
                tilemapSheets.emplace_back(sheetData.sheet->getName(), sheetData.sheet->handle);

            Ref<Texture2DSheet> tilemapSheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet);
            Widgets::Combobox("Tilemap Sheets", tilemapSheet ? tilemapSheet->getName() : "None", mCurrentSheet, tilemapSheets.data(), tilemapSheets.size());

            // All Sheets
            std::vector<SheetEntry> allSheets;
            for (AssetHandle sheetHandle : AssetManager::GetAssetsWithType(AssetType::TextureSheet))
            {
                if (HasSheetEntry(sheetHandle, tilemapSheets))
                    continue;

                Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(sheetHandle);
                allSheets.emplace_back(sheet->getName(), sheetHandle);
            }

            Ref<Texture2DSheet> sheetToAdd = AssetManager::GetAsset<Texture2DSheet>(mSheetToAdd);
            Widgets::Combobox("Add", sheetToAdd ? sheetToAdd->getName() : "None", mSheetToAdd, allSheets.data(), allSheets.size());
            Widgets::SameLine();
            Widgets::Button("+", [this]() { mTilemap->addSheet(mSheetToAdd); });
        }

        DrawSheet();
        Widgets::EndGroup();
    }

    void MapEditModal::DrawSheet()
    {
        Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet);
        const glm::vec2& startPos = ImGuiUtils::CursorPos();

        // Sprite Sheet
        glm::vec2 sheetImageSize = ImGuiUtils::AvailableRegion();
        sheetImageSize.y -= 10 * mFrameHeightWithSpacing;

        Ref<IRenderable> sheetImage = EditorResources::NoTexture;
        if (sheet)
            sheetImage = sheet;
        Widgets::Image(sheetImage, sheetImageSize);

        // Selected Subtexture
        auto subtexImageSize = ImGuiUtils::AvailableRegion();
        subtexImageSize.x *= 0.5f;
        subtexImageSize.y -= (3 * mFrameHeightWithSpacing + 0.25f * subtexImageSize.y);

        Ref<IRenderable> subTexImage = EditorResources::NoTexture;
        if (mBrushSubtex.valid())
            subTexImage = mTilemap->getTileTex(mBrushSubtex.textureID);

        glm::vec2 pos = ImGuiUtils::CursorPos();
        pos.x += 0.25f * subtexImageSize.x;
        pos.y += 0.25f * subtexImageSize.y;
        ImGuiUtils::SetCursorPos(pos);
        Widgets::Image(subTexImage, subtexImageSize, mBrushSubtex.rotation);

        Widgets::SameLine();
        ImGuiUtils::SetCursorPosY(pos.y);
        Widgets::Button("Clear", [this]() { mBrushSubtex = {}; });
        Widgets::Button("Rotate Right", [this]() { mBrushSubtex.rotation = std::fmod(mBrushSubtex.rotation + Angle::Rad270f, Angle::Rad360f); });
        Widgets::Button("Rotate Left", [this]() { mBrushSubtex.rotation = std::fmod(mBrushSubtex.rotation + Angle::Rad90f, Angle::Rad360f); });

        if (!sheet)
            return;

        // Selection Buttons
        u32 tileCountX = sheet->getTileCountX();
        u32 tileCountY = sheet->getTileCountY();

        ImGuiUtils::SetButtonTransparent();

        Widgets::GridControl(startPos, sheetImageSize, tileCountX, tileCountY, [&](const GridPosition& pos, const glm::vec2& elementSize)
        {
            std::string name = fmt::format("##SheetTile({}, {})", pos.x, pos.y);
            Widgets::Button(name, elementSize, [&]()
            {
                mBrushSubtex = TileRenderData(GetStartIndex(sheet, mTilemap->getSheets()) + sheet->getPositionIndex(pos));
            });
        });

        ImGuiUtils::ResetButtonTransparency();
    }

    bool MapEditModal::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.repeat)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        if (!control)
            return false;

        switch (e.keyCode)
        {
        case Key::Q:
            mEditMode = EditMode::Paint;
            return true;
        case Key::W:
            mEditMode = EditMode::Selection;
            return true;
        }

        return false;
    }
}