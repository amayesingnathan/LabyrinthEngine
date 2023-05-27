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
        const glm::vec2& panelSize = Utils::AvailableRegion<glm::vec2>();
        Ref<IRenderable> sheetImage = EditorResources::NoTexture;
        if (mCurrentSheet)
            sheetImage = mCurrentSheet;

        LabWidgets::ImageButton(sheetImage, glm::vec2{panelSize.x - 15.0f, 200.0f});

        bool toDelete = false;
        Widgets::BeginContextPopup();
        Widgets::AddContextItem("Delete", [&]() { toDelete = true; });
        Widgets::EndContextPopup();

        Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [this](const fs::path& var)
        {
            fs::path extension = var.extension();

            if (AssetManager::IsExtensionValid(extension, AssetType::Texture))
            {
                mNewSheetWidth = 0;
                mNewSheetHeight = 0;

                ModalManager::Open<NewSpritesheetModal>("New Spritesheet...", ModalButtons::OKCancel, mCurrentSheet, var);
                ModalManager::AddCallback([this] { mCurrentSubTex = nullptr; });
            }
            else if (AssetManager::IsExtensionValid(extension, AssetType::TextureSheet))
            {
                mCurrentSheet = AssetManager::GetAsset<Texture2DSheet>(var);
                mCurrentSubTex = nullptr;
            }
        });

        std::vector<SubTexEntry> comboEntries;
        comboEntries.emplace_back(NO_NAME, nullptr);
        if (mCurrentSheet)
        {
            for (AssetHandle handle : mCurrentSheet->getSubTextures())
            {
                Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(handle);
                comboEntries.emplace_back(subtex->getName(), subtex);
            }
        }

        std::string_view subTexName = mCurrentSubTex ? mCurrentSubTex->getName() : NO_NAME;
        Widgets::Combobox<Ref<SubTexture2D>>("Subtextures", subTexName, mCurrentSubTex, comboEntries);

        Widgets::NewLine();

        Ref<IRenderable> subtexImage = EditorResources::NoTexture;
        if (mCurrentSubTex)
            subtexImage = mCurrentSubTex;
        LabWidgets::Image(subtexImage, glm::vec2{panelSize.x - 15.0f, 200.0f});
        if (mCurrentSubTex)
            Widgets::AddDragDropSource("SPRITE_SHEET_ITEM", mCurrentSubTex->handle);

        Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [&](const fs::path& var)
        {
            if (AssetManager::IsExtensionValid(var.extension(), AssetType::SubTexture))
            {
                mCurrentSubTex = AssetManager::GetAsset<SubTexture2D>(var);
                mCurrentSheet = mCurrentSubTex->getSheet();
            }
        });

        if (toDelete && mCurrentSheet)
        {
            for (AssetHandle subtex : mCurrentSheet->getSubTextures())
                AssetManager::DestroyAsset(subtex);
            FileUtils::RemoveDir(Project::GetAssetDirectory() / SubTexture2D::GetAssetDirectory() / mCurrentSheet->getName());
            AssetManager::DestroyAsset(mCurrentSheet->handle);
            mCurrentSheet = nullptr;
        }
    }
}
