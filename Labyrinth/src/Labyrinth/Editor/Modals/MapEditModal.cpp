#include "Lpch.h"
#include "MapEditModal.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Tools/EnumUtils.h>

namespace Laby {

    using SheetEntry = ComboEntry<AssetHandle>;

    static bool HasSheetEntry(AssetHandle sheet, const std::vector<SheetEntry>& entries)
    {
        return std::find_if(entries.begin(), entries.end(), [sheet](const SheetEntry& entry) { return entry.value == sheet; }) != entries.end();
    }

    static TileID GetStartIndex(Ref<Texture2DSheet> sheet, const std::vector<SheetData>& sheets)
    {
        return std::find_if(sheets.begin(), sheets.end(), [&](const SheetData& data) { return data.sheet == sheet; })->startIndex;
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
            if (mHoveredTile.valid() && mHoveredTex != mCurrentTex)
                mTilemap->setTile(mCurrentLayer, mHoveredTile, mCurrentTex.textureID, mCurrentTex.rotation);
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
            Widgets::Button(name.c_str(), elementSize);
            Widgets::OnWidgetHovered([&]()
            {
                mHoveredTile = pos;
                mHoveredTex = mTilemap->getTile(mCurrentLayer, pos);
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
        if (mCurrentTex.valid())
            subTexImage = mTilemap->getTileTex(mCurrentTex.textureID);

        glm::vec2 pos = ImGuiUtils::CursorPos();
        pos.x += 0.25f * subtexImageSize.x;
        pos.y += 0.25f * subtexImageSize.y;
        ImGuiUtils::SetCursorPos(pos);
        Widgets::Image(subTexImage, subtexImageSize, mCurrentTex.rotation);

        Widgets::SameLine();
        ImGuiUtils::SetCursorPosY(pos.y);
        Widgets::Button("Clear", [this]() { mCurrentTex = TileRenderData(); });
        Widgets::Button("Rotate Right", [this]() { mCurrentTex.rotation = std::fmod(mCurrentTex.rotation + Angle::Rad270f, Angle::Rad360f); });
        Widgets::Button("Rotate Left", [this]() { mCurrentTex.rotation = std::fmod(mCurrentTex.rotation + Angle::Rad90f, Angle::Rad360f); });

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
                mCurrentTex = TileRenderData(GetStartIndex(sheet, mTilemap->getSheets()) + sheet->getPositionIndex(pos));
            });
        });

        ImGuiUtils::ResetButtonTransparency();
    }

    bool MapEditModal::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.repeat)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

        switch (e.keyCode)
        {
        case Key::Q:
        {
            if (control)
            {
                mEditMode = EditMode::Paint;
                return true;
            }
            break;
        }
        case Key::W:
        {
            if (control)
            {
                mEditMode = EditMode::Physics;
                return true;
            }
            break;
        }
        case Key::E:
        {
            if (control)
            {
                mEditMode = EditMode::Scripts;
                return true;
            }
            break;
        }
        }

        return false;
    }
}