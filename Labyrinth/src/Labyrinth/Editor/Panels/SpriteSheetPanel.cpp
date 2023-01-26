#include "Lpch.h"
#include "SpriteSheetPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

    static constexpr std::string_view NO_NAME = "None";

    using SubTexEntry = ComboEntry<Ref<SubTexture2D>>;

    void SpriteSheetPanel::onImGuiRender()
    {
        const glm::vec2& panelSize = ImGuiUtils::AvailableRegion();
        Ref<IRenderable> sheetImage = EditorResources::NoTexture;
        if (mCurrentSheet)
            sheetImage = mCurrentSheet;

        Widgets::Image(sheetImage, { panelSize.x - 15.0f, 200.0f });

        Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [&](const fs::path& var)
        {
            fs::path extension = var.extension();

            if (AssetManager::IsExtensionValid(extension, AssetType::Texture))
            {
                mNewSheetWidth = 0;
                mNewSheetHeight = 0;

                ModalManager::OpenInline("New Spritesheet...", ModalButtons::OKCancel, [&]()
                {
                    Widgets::Label("Enter the tile size for this sprite sheet");
                    Widgets::NewLine();

                    Widgets::StringEdit("Name", mNewSheetName);
                    Widgets::UIntEdit("Width", mNewSheetWidth);
                    Widgets::UIntEdit("Height", mNewSheetHeight);
                }, [&]()
                {
                    if (mNewSheetWidth == 0 || mNewSheetHeight == 0)
                        return;

                    mCurrentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mNewSheetName, fmt::format("spritesheets/{}", mNewSheetName),
                        mNewSheetName, var, glm::vec2{ (f32)mNewSheetWidth, (f32)mNewSheetHeight });

                    mCurrentSubTex = nullptr;
                });
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
        Widgets::Combobox("Subtextures", subTexName, mCurrentSubTex, comboEntries.data(), comboEntries.size());

        Widgets::NewLine();

        Ref<IRenderable> subtexImage = EditorResources::NoTexture;
        if (mCurrentSubTex)
            subtexImage = mCurrentSubTex;
        Widgets::Image(subtexImage, { panelSize.x - 15.0f, 200.0f });
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
    }
}
