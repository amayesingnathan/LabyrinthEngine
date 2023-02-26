#include "Lpch.h"
#include "NewAnimationModal.h"

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

    using TextureSheetEntry = imcpp::ComboEntry<Ref<Texture2DSheet>>;

    void NewAnimationModal::onImGuiRender()
	{
        if (!mNewAnimation.mFrames.empty())
            mNewAnimation.step();

        std::vector<TextureSheetEntry> tilemapSheets;
        for (Ref<Texture2DSheet> sheet : AssetManager::GetAssetsWithType<Texture2DSheet>())
            tilemapSheets.emplace_back(sheet->getName(), sheet);

        Widgets::Combobox<Ref<Texture2DSheet>>("Texture Sheets", mSheet ? mSheet->getName() : "None", mSheet, tilemapSheets);

        Widgets::Disable(!mSheet);

        Widgets::Label("Please enter a name for the animation:");
        Widgets::NewLine();
        Widgets::StringEdit("Name", mNewAnimation.mName);

        glm::vec2 cursorPos = Utils::CursorPos<glm::vec2>();
        glm::vec2 region = Utils::AvailableRegion<glm::vec2>();

        glm::vec2 mapSize = 0.75f * region;
        LabWidgets::Image(mSheet, mapSize);
        imcpp::GridFunction<glm::vec2> gridFunc = [this](u32 x, u32 y, const glm::vec2& tileSize)
        {
            Widgets::Button(tileSize, std::format("##SheetTiles({}, {})", x, y), [=, this]()
            {
                mFrameInProgress.sprite = mSheet->getFromPosition({x, y});
            });
        };

        Widgets::SameLine();

        if (mSheet)
        {
            glm::vec2 tempCursorPos = Utils::CursorPos<glm::vec2>();
            Utils::SetButtonTransparent();
            Widgets::GridControl(cursorPos, mapSize, mSheet->getTileCountX(), mSheet->getTileCountY(), gridFunc);
            Utils::ResetButtonTransparency();
            Utils::SetCursorPos(tempCursorPos.x, tempCursorPos.y);
        }

        Widgets::BeginChild("Selected Tile", glm::vec2{ 0, mapSize.y });

        glm::vec2 tileRegion = Utils::AvailableRegion<glm::vec2>();
        LabWidgets::Image(mFrameInProgress.sprite ? AssetManager::GetAsset<SubTexture2D>(mFrameInProgress.sprite) : nullptr, {tileRegion.x, tileRegion.x});

        Widgets::UIntEdit("Frame Length", mFrameInProgress.frameLength);
        Widgets::Disable(!mFrameInProgress.valid());
        Widgets::Button("Add", [this]() { mNewAnimation.addFrame(std::move(mFrameInProgress)); mFrameInProgress = AnimationFrame(); });
        Widgets::EndDisable();

        Widgets::EndChild();

        f32 bottomPanelIndent = 0.0625f * Utils::AvailableRegion().x;
        Widgets::BeginChild("Current Animation", glm::vec2{ -bottomPanelIndent, -2 * Utils::FrameHeightWithSpacing() });
        f32 animationRegion = 0.0625f * Utils::AvailableRegion().x;

        Widgets::BeginColumns(16);

        animationRegion /= ((mNewAnimation.mFrames.size() / 16) + 1);

        for (const AnimationFrame& frame : mNewAnimation)
        {
            LabWidgets::Image(AssetManager::GetAsset<SubTexture2D>(frame.sprite), { animationRegion, animationRegion });

            Widgets::NextColumn();
        }

        Widgets::EndColumns();

        Widgets::EndChild();

        Widgets::SameLine();

        LabWidgets::Image(AssetManager::GetAsset<SubTexture2D>(mNewAnimation.currentFrame()), { bottomPanelIndent, bottomPanelIndent });

        Widgets::EndDisable();
	}

	void NewAnimationModal::onCustomButtonRender(bool& open)
    {
        Widgets::Disable(!mSheet);
        Widgets::Button("Create", [&, this]()
        {
            onComplete();
            open = false;
        });
        Widgets::EndDisable();

        Widgets::SameLine();

        Widgets::Button("Cancel", [&, this]()
        {
            open = false;
        });
    }

	void NewAnimationModal::onComplete()
	{
		Ref<Animation> newAnimation = AssetManager::CreateNewAsset<Animation>(mNewAnimation.mName, mNewAnimation.mName);
        newAnimation->addFrames(std::move(mNewAnimation.mFrames));
        AssetImporter::Serialise(newAnimation);
        AssetManager::ReloadData(newAnimation->handle);
	}
}