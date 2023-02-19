#include "Lpch.h"
#include "MapEditModal.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Tools/EnumUtils.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

    using SheetEntry = imcpp::ComboEntry<AssetHandle>;
    using ScriptClassEntry = imcpp::ComboEntry<Ref<ScriptClass>>;

    static bool HasSheetEntry(AssetHandle sheet, const std::vector<SheetEntry>& entries)
    {
        return std::ranges::find_if(entries, [sheet](const SheetEntry& entry) { return entry.value == sheet; }) != entries.end();
    }

    static TileID GetStartIndex(Ref<Texture2DSheet> sheet, const std::vector<SheetData>& sheets)
    {
        auto it = std::ranges::find_if(sheets, [&](const SheetData& data) { return data.sheet == sheet; });
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
        if (Utils::IsKeyPressed(Key::LeftControl) || Utils::IsKeyPressed(Key::RightControl))
        {
            if (Utils::IsKeyPressed(Key::Q))
                mEditMode = EditMode::Paint;
            else if (Utils::IsKeyPressed(Key::W))
                mEditMode = EditMode::Selection;
        }

        if (mEditMode == EditMode::Paint && Utils::IsMouseDown(Mouse::ButtonLeft))
        {
            mCurrentlyPainting = true;
            if (mHoveredTile.valid() && mHoveredSubtex != mBrushSubtex)
                mTilemap->setTileData(mCurrentLayer, mHoveredTile, mBrushSubtex);
        }
        if (mCurrentlyPainting && Utils::IsMouseReleased(Mouse::ButtonLeft))
            mCurrentlyPainting = false;

        mFrameHeightWithSpacing = Utils::FrameHeightWithSpacing();

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

    void MapEditModal::LeftPane()
    {
        Widgets::BeginChild<glm::vec2>("Left Pane", glm::vec2{ 300, -3 * mFrameHeightWithSpacing }, false);

        Widgets::BeginGroup();

        Widgets::Label("Layers");
        Widgets::Separator();
        Widgets::BeginChild("Layers", glm::vec2{ 0, -20 * mFrameHeightWithSpacing });
        usize layerIndex = 0;
        for (const auto& layer : mTilemap->getLayers())
        {
            std::string layerLit = fmt::format("Layer {}", layerIndex + 1);
            Widgets::Selectable(layerLit.c_str(), mCurrentLayer == layerIndex, [this, layerIndex]() { mCurrentLayer = layerIndex; });
            layerIndex++;
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

        glm::vec2 imageSize = 0.25f * Utils::AvailableRegion<glm::vec2>();

        Widgets::BeginChild("Selected", glm::vec2{ 0, -10 * mFrameHeightWithSpacing });
        Widgets::Label(fmt::format("Tile: ({}, {})", mCurrentTile.x, mCurrentTile.y));

        bool validCurrentTile = mCurrentTile.valid();
        TileBehaviourData currentTileData = mTilemap->getTileBehaviour(mCurrentTile);

        std::vector<ScriptClassEntry> comboEntries;
        comboEntries.emplace_back("##NoScript", nullptr);
        for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
            comboEntries.emplace_back(key, klass);

        Widgets::Disable(!validCurrentTile);

        Ref<ScriptClass> scriptClass = ScriptEngine::GetAppClass(currentTileData.script);
        Widgets::Combobox<Ref<ScriptClass>>("Behaviour", currentTileData.script, scriptClass, comboEntries,
            [&](std::string_view name, Ref<ScriptClass> klass) { currentTileData.script = name; });

        Widgets::Checkbox("Solid", currentTileData.solid);
        if (validCurrentTile)
            mTilemap->setTileBehaviour(mCurrentTile, currentTileData);

        Widgets::EndDisable();

        {
            Ref<IRenderable> image = EditorResources::NoTexture;
            f32 rotation = 0.0f;
            if (mCurrentTile.valid())
            {
                image = mTilemap->getTileTex(mCurrentSubtex.textureID);
                rotation = mCurrentSubtex.rotation;
            }

            glm::vec2 newPos = Utils::CursorPos<glm::vec2>() + 0.2f * imageSize;
            Utils::SetCursorPos(newPos.x, newPos.y);
            LabWidgets::Image(image, imageSize, rotation);
        }

        Widgets::EndChild(); // Selected


        Widgets::BeginChild("Hovered");

        Widgets::Disable();

        Widgets::Label(fmt::format("Hovered Tile: ({}, {})", mHoveredTile.x, mHoveredTile.y));
        Widgets::Label(fmt::format("Script: {}", mHoveredBehaviour.script));
        Widgets::Checkbox("Solid", mHoveredBehaviour.solid);

        Widgets::EndDisable();

        {
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

            glm::vec2 newPos = Utils::CursorPos<glm::vec2>() + 0.2f * imageSize;
            Utils::SetCursorPos(newPos.x, newPos.y);
            LabWidgets::Image(image, imageSize, rotation);
        }

        Widgets::EndChild(); // Hovered

        Widgets::EndChild(); // Left Pane
    }

    void MapEditModal::CentrePane()
    {
        Widgets::BeginChild("Map", glm::vec2{ -400, -3 * mFrameHeightWithSpacing });

        const glm::vec2& pos = Utils::CursorPos<glm::vec2>();

        auto imageSize = Utils::AvailableRegion<glm::vec2>();
        imageSize.y -= mFrameHeightWithSpacing;
        LabWidgets::Image(mTilemap, imageSize);
        Widgets::OnWidgetHovered([]() {}, [&]()
        {
            mHoveredTile = GridPosition();
            mHoveredSubtex = TileRenderData();
            mHoveredBehaviour = TileBehaviourData();
        });

        Widgets::Label(fmt::format("Edit Mode: {}", Enum::ToString(mEditMode)));
        Widgets::SameLine();
        Utils::SetCursorPosX(pos.x + (0.2f * imageSize.x));
        Widgets::Label("Ctrl + Q: Paint Mode");
        Widgets::SameLine();
        Utils::SetCursorPosX(pos.x + (0.4f * imageSize.x));
        Widgets::Label("Ctrl + W: Select Mode");
        Widgets::SameLine();
        Utils::SetCursorPosX(pos.x + (0.75f * imageSize.x));
        Widgets::Checkbox("Colliders", mDisplayColliders);

        if (!mTilemap->hasLayers())
        {
            Widgets::EndChild();
            return;
        }

        Utils::SetButtonTransparent();
        imcpp::GridFunction<glm::vec2> gridFunc = [this](u32 x, u32 y, const glm::vec2& elementSize)
        {
            GridPosition pos = { x, y };
            std::string name = fmt::format("##MapTiles({}, {})", x, y);
            Widgets::Button(elementSize, name.c_str(), [&]() 
            { 
                mCurrentTile = pos;
                mCurrentSubtex = mTilemap->getTileData(mCurrentLayer, pos);
                mCurrentBehaviour = mTilemap->getTileBehaviour(pos);
            });
            Widgets::OnWidgetHovered([&]()
            {
                mHoveredTile = pos;
                mHoveredSubtex = mTilemap->getTileData(mCurrentLayer, pos);
                mHoveredBehaviour = mTilemap->getTileBehaviour(pos);
            });
        };

        Widgets::GridControl(pos, imageSize, mMapWidth, mMapHeight, gridFunc);
        Utils::ResetButtonTransparency();

        Widgets::EndChild();
    }

    void MapEditModal::RightPane()
    {
        Widgets::BeginGroup();
        Widgets::BeginChild<glm::vec2>("Sheets", { 0, 200 });

        {   // Sheets on Tilemap
            std::vector<SheetEntry> tilemapSheets;
            for (const SheetData& sheetData : mTilemap->getSheets())
                tilemapSheets.emplace_back(sheetData.sheet->getName(), sheetData.sheet->handle);

            Ref<Texture2DSheet> tilemapSheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet);
            Widgets::Combobox<AssetHandle>("Tilemap Sheets", tilemapSheet ? tilemapSheet->getName() : "None", mCurrentSheet, tilemapSheets);

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
            Widgets::Combobox<AssetHandle>("Add", sheetToAdd ? sheetToAdd->getName() : "None", mSheetToAdd, allSheets);
            Widgets::SameLine();
            Widgets::Button("+", [this]() { mTilemap->addSheet(mSheetToAdd); mSheetToAdd = 0; });
        }
        Widgets::EndChild();

        DrawSheet();
        Widgets::EndGroup();
    }

    void MapEditModal::DrawSheet()
    {
        Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet);
        const glm::vec2& startPos = Utils::CursorPos<glm::vec2>();

        // Sprite Sheet
        glm::vec2 sheetImageSize = Utils::AvailableRegion<glm::vec2>();
        sheetImageSize.y -= 10 * mFrameHeightWithSpacing;

        Ref<IRenderable> sheetImage = EditorResources::NoTexture;
        if (sheet)
            sheetImage = sheet;
        LabWidgets::Image(sheetImage, sheetImageSize);

        // Selected Subtexture
        auto subtexImageSize = Utils::AvailableRegion<glm::vec2>();
        subtexImageSize.x *= 0.5f;
        subtexImageSize.y -= (3 * mFrameHeightWithSpacing + 0.25f * subtexImageSize.y);

        Ref<IRenderable> subTexImage = EditorResources::NoTexture;
        if (mBrushSubtex.valid())
            subTexImage = mTilemap->getTileTex(mBrushSubtex.textureID);

        glm::vec2 pos = Utils::CursorPos<glm::vec2>();
        pos.x += 0.25f * subtexImageSize.x;
        pos.y += 0.25f * subtexImageSize.y;
        Utils::SetCursorPos(pos.x, pos.y);
        LabWidgets::Image(subTexImage, subtexImageSize, mBrushSubtex.rotation);

        Widgets::SameLine();
        Utils::SetCursorPosY(pos.y);
        glm::vec2 buttonPos = Utils::CursorPos<glm::vec2>();
        Widgets::Button("Clear", [this]() { mBrushSubtex = {}; });

        buttonPos.y += mFrameHeightWithSpacing;
        Utils::SetCursorPos(buttonPos.x, buttonPos.y);
        Widgets::Button("Rotate Right", [this]() { mBrushSubtex.rotation = std::fmod(mBrushSubtex.rotation + Angle::Rad270f, Angle::Rad360f); });

        buttonPos.y += mFrameHeightWithSpacing;
        Utils::SetCursorPos(buttonPos.x, buttonPos.y);
        Widgets::Button("Rotate Left", [this]() { mBrushSubtex.rotation = std::fmod(mBrushSubtex.rotation + Angle::Rad90f, Angle::Rad360f); });

        if (!sheet)
            return;

        // Selection Buttons
        u32 tileCountX = sheet->getTileCountX();
        u32 tileCountY = sheet->getTileCountY();

        Utils::SetButtonTransparent();

        imcpp::GridFunction<glm::vec2> gridFunc = [this, sheet](u32 x, u32 y, const glm::vec2& elementSize)
        {
            GridPosition pos = { x,y };
            std::string name = fmt::format("##SheetTile({}, {})", x, y);
            Widgets::Button(elementSize, name, [&]()
            {
                mBrushSubtex = TileRenderData(GetStartIndex(sheet, mTilemap->getSheets()) + sheet->getPositionIndex(pos));
            });
        };
        Widgets::GridControl(startPos, sheetImageSize, tileCountX, tileCountY, gridFunc);

        Utils::ResetButtonTransparency();
    }
}