#include "Lpch.h"
#include "AnimationEditModal.h"

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/SubTexture.h>
#include <Labyrinth/Tools/EnumUtils.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	using TextureSheetEntry = imcpp::ComboEntry<Ref<Texture2DSheet>>;

	AnimationEditModal::AnimationEditModal(EditingMode mode, Ref<Animation> animation)
		: mWindowMode(mode)
	{
		switch (mWindowMode)
		{
		case EditingMode::Add:
			mAnimation = Ref<Animation>::Create("New Animation");
			break;
		case EditingMode::Edit:
			mAnimation = animation ? AnimationUtils::Clone(animation) : nullptr;
			break;
		}

		if (!mAnimation)
		{
			LAB_CORE_ERROR("Failed to initialise animation.");
			return;
		}
	}

	void AnimationEditModal::onImGuiRender()
	{
		if (!mAnimation)
			return;

        if (!mAnimation->mFrames.empty())
            mAnimation->step();

		glm::vec2 availableRegion = Utils::AvailableRegion<glm::vec2>();
		bool validFrame = mSelectedFrame.valid();

		{
			glm::vec2 currentFrameRegion = 0.75f * availableRegion;
			Widgets::BeginChild("SelectedFrame", currentFrameRegion, true);

			Ref<SubTexture2D> selectedFrame = AssetManager::GetAsset<SubTexture2D>(mSelectedFrame.sprite);
			if (selectedFrame)
				mCurrentSheet = selectedFrame->getSheet();

			std::vector<TextureSheetEntry> tilemapSheets;
			for (Ref<Texture2DSheet> sheet : AssetManager::GetAssetsWithType<Texture2DSheet>())
				tilemapSheets.emplace_back(sheet->getName(), sheet);
			Widgets::Combobox<Ref<Texture2DSheet>>("Texture Sheets", mCurrentSheet ? mCurrentSheet->getName() : "None", mCurrentSheet, tilemapSheets);

			Widgets::BeginChild("SelectedFrame_Sheet", glm::vec2{ 0.5f * currentFrameRegion.x, 0 });

			LabWidgets::TextureSheet("AnimationEditSheet", mCurrentSheet, Utils::AvailableRegion<glm::vec2>(), [=, this](AssetHandle handle) 
			{
				if (!validFrame)
				{
					mFrameMode = EditingMode::Add;
					mSelectedFrame = AnimationFrame();
					mSelectedFrame.id = UUID();
				}

				mSelectedFrame.sprite = handle;
			});
			Widgets::EndChild();

			Widgets::SameLine();

			Widgets::BeginChild("SelectedFrame_Control", Zero::Vec2);
			Widgets::Disable(!validFrame);

			Widgets::BeginChild("SelectedFrame_Control_Subtex", glm::vec2{ 0, -2 * Utils::FrameHeightWithSpacing() });
			LabWidgets::Image(selectedFrame, Utils::AvailableRegion<glm::vec2>());
			Widgets::EndChild();

			Widgets::BeginChild("SelectedFrame_Control_Controls", Zero::Vec2);
			static usize sDisplayOnDisabled = 0;
			Widgets::UIntEdit("Length", validFrame ? mSelectedFrame.length : sDisplayOnDisabled);

			if (mFrameMode == EditingMode::Edit)
				mAnimation->overwriteFrame(mSelectedFrame);

			Widgets::Disable(mFrameMode != EditingMode::Add || mSelectedFrame.length == 0);
			Widgets::Button("Add", [this]() 
			{ 
				mAnimation->addFrame(mSelectedFrame);
				mSelectedFrame = AnimationFrame();
				mSelectedFrame.id = UUID();
			});
			Widgets::EndDisable();

			Widgets::SameLine();

			Widgets::Disable(mFrameMode != EditingMode::Edit);
			Widgets::Button("Remove", [this]()
			{
				auto& frames = mAnimation->getFrames();
				std::erase_if(frames, [this](const auto& frame) { return frame.id == mSelectedFrame.id; });
				mSelectedFrame = AnimationFrame();
				mFrameMode = EditingMode::None;
			});
			Widgets::EndDisable();

			Widgets::SameLine(0, 20.0f);
			Widgets::Label(std::format("Editing Mode: {}", Enum::ToString(mFrameMode)));

			Widgets::EndChild();

			Widgets::EndDisable();
			Widgets::EndChild();

			Widgets::EndChild();
		}
		Widgets::SameLine();
		{
			glm::vec2 rightPanelSize = std::invoke([](const glm::vec2& vec) { return glm::vec2{ vec.x, 0.75f * vec.y }; }, Utils::AvailableRegion<glm::vec2>());
			Widgets::BeginChild("Preview", rightPanelSize, true);

			LabWidgets::Image(AssetManager::GetAsset<SubTexture2D>(mAnimation->currentFrame()), glm::vec2{ rightPanelSize.x, rightPanelSize.y - 2 * Utils::FrameHeightWithSpacing() });

			Widgets::StringEdit("Name", mAnimation->mName);

			Widgets::EndChild();
		}

		{
			Widgets::BeginChild("Timeline", glm::vec2{ 0, -2 * Utils::FrameHeightWithSpacing() });

			glm::vec2 timelineEntrySize = std::invoke([](const glm::vec2& region) { return glm::vec2{ region.y, region.y }; }, 0.9f * Utils::AvailableRegion<glm::vec2>());

			auto& frames = mAnimation->getFrames();
			for (usize i = 0; i < frames.size(); i++)
			{
				AnimationFrame& currFrame = frames[i];
				Utils::PushID(std::format("{} - Frame {}", mAnimation->getName(), i));
				LabWidgets::ImageButton(AssetManager::GetAsset<SubTexture2D>(currFrame.sprite), timelineEntrySize, [this, &currFrame]()
				{ 
					mSelectedFrame = currFrame;
					mFrameMode = EditingMode::Edit;
				});

				Widgets::AddDragDropSource("FRAME_ITEM", currFrame);
				Widgets::AddDragDropTarget<AnimationFrame>("FRAME_ITEM", [&, this](const auto& framePayload)
				{
					auto currIt = std::ranges::find_if(frames, [&](const auto& frame) { return frame.id == currFrame.id;});
					auto droppedIt = std::ranges::find_if(frames, [&](const auto& frame) { return frame.id == framePayload.id;});
					std::swap(currIt, droppedIt);
				});
				Utils::PopID();

				Widgets::SameLine(0, 5.0f);
			}

			Widgets::EndChild();
		}
	}

	void AnimationEditModal::onCustomButtonRender(bool& open)
	{
		Widgets::Button("New Frame", [this]()
		{
			mSelectedFrame = AnimationFrame();
			mFrameMode = EditingMode::Add;
		});

		Widgets::SameLine();

		Widgets::Button("OK", [&, this]()
		{
			onComplete();
			open = false;
		});

		Widgets::SameLine();

		Widgets::Button("Cancel", [&, this]()
		{
			open = false;
		});
	}

	void AnimationEditModal::onComplete()
	{
		switch (mWindowMode)
		{
		case EditingMode::Add:
		{
			Ref<Animation> newAnimation = AssetManager::CreateNewAsset<Animation>(mAnimation->mName, mAnimation->mName);
			newAnimation->addFrames(std::move(mAnimation->mFrames));
			AssetImporter::Serialise(newAnimation);
			AssetManager::ReloadData(newAnimation->handle);
			break;
		}
		case EditingMode::Edit:
		{
			AssetImporter::Serialise(mAnimation);
			AssetManager::ReloadData(mAnimation->handle);
			break;
		}
		}
	}
}