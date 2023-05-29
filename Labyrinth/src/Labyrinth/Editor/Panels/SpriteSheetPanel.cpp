#include "Lpch.h"
#include "SpriteSheetPanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/Modals/NewSpritesheetModal.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

    static constexpr std::string_view NO_NAME = "None";

    using SubTexEntry = imcpp::ComboEntry<Ref<SubTexture2D>>;

    void SpriteSheetPanel::onImGuiRender()
    {
        Ref<Texture2DSheet> sheet = AssetManager::GetAsset<Texture2DSheet>(mCurrentSheet);
        Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(mCurrentSubTex);

        const glm::vec2& panelSize = Utils::AvailableRegion<glm::vec2>();
        Ref<IRenderable> sheetImage = EditorResources::NoTexture;
        if (sheet)
            sheetImage = sheet;

        LabWidgets::ImageButton(sheetImage, glm::vec2{panelSize.x - 15.0f, 200.0f});

        bool toDelete = false;
        Widgets::BeginContextPopup();
        Widgets::AddContextItem("Delete", [&]() { toDelete = true; });
        Widgets::EndContextPopup();

        Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [=](const fs::path& var)
        {
            fs::path extension = var.extension();

            if (AssetManager::IsExtensionValid(extension, AssetType::Texture))
            {
                mNewSheetWidth = 0;
                mNewSheetHeight = 0;

                ModalManager::Open<NewSpritesheetModal>("New Spritesheet...", ModalButtons::Custom, mCurrentSheet, var);
            }
            else if (AssetManager::IsExtensionValid(extension, AssetType::TextureSheet))
            {
                mCurrentSheet = AssetManager::GetHandleFromPath(var);
                mCurrentSubTex = 0;
            }
        });

        std::vector<SubTexEntry> comboEntries;
        comboEntries.emplace_back(NO_NAME, nullptr);
        if (sheet)
        {
            for (AssetHandle handle : sheet->getSubTextures())
            {
                Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(handle);
                comboEntries.emplace_back(subtex->getName(), subtex);
            }
        }

        std::string_view subTexName = subtex ? subtex->getName() : NO_NAME;
        Widgets::Combobox<Ref<SubTexture2D>>("Subtextures", subTexName, subtex, comboEntries);

        Widgets::NewLine();

        Ref<IRenderable> subtexImage = EditorResources::NoTexture;
        if (subtex)
            subtexImage = subtex;
        LabWidgets::Image(subtexImage, glm::vec2{panelSize.x - 15.0f, 200.0f});
        if (subtex)
            Widgets::AddDragDropSource("SPRITE_SHEET_ITEM", subtex->handle);

        Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [&](const fs::path& var)
        {
            if (AssetManager::IsExtensionValid(var.extension(), AssetType::SubTexture))
            {
                mCurrentSubTex = AssetManager::GetHandleFromPath(var);
                Ref<SubTexture2D> newSubtex = AssetManager::GetAsset<SubTexture2D>(mCurrentSubTex);
                mCurrentSheet = newSubtex ? newSubtex->getSheet()->handle : AssetHandle(0);
            }
        });

        if (toDelete && sheet)
        {
            for (AssetHandle subtex : sheet->getSubTextures())
                AssetManager::DestroyAsset(subtex);
            FileUtils::RemoveDir(Project::GetAssetDirectory() / SubTexture2D::GetAssetDirectory() / sheet->getName());
            AssetManager::DestroyAsset(sheet->handle);
            mCurrentSheet = 0;
        }
    }
}
