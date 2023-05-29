#include "Lpch.h"
#include "NewSpritesheetModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

#include "SubTexturePickerModal.h"

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	NewSpritesheetModal::NewSpritesheetModal(AssetHandle& returnSheet, const fs::path& filepath)
		: mReturnSheet(&returnSheet), mTexturePath(filepath)
	{
	}

	void NewSpritesheetModal::onImGuiRender()
	{
		Widgets::Label("Enter the tile size for this sprite sheet");
		Widgets::NewLine();

		Widgets::StringEdit("Name", mSheetName);
		Widgets::UIntEdit("Tile Width (px)", mTileWidth);
		Widgets::UIntEdit("Tile Height (px)", mTileHeight);
	}

	void NewSpritesheetModal::onCustomButtonRender(bool& open)
	{
		Widgets::Button("Generate", [&, this]()
		{
			onComplete();
			Ref<Texture2DSheet> newSheet = AssetManager::SaveMemoryOnlyAsset<Texture2DSheet>(mSheetName, mInProgressSheet);
			newSheet->generateTileset();
			*mReturnSheet = newSheet->handle;
			open = false;
		});

		Widgets::SameLine();

		Widgets::Button("Pick", [=, &open]()
		{
			onComplete();

			AssetHandle* returnSheetCopy = mReturnSheet;
			AssetHandle inProgressSheetCopy = mInProgressSheet;
			Application::SubmitActionToMainThread([=]
			{
				ModalManager::Open<SubTexturePickerModal>("Select subtextures...", ModalButtons::Custom, *returnSheetCopy, inProgressSheetCopy);
			});
			open = false;
		});

		Widgets::SameLine();

		Widgets::Button("Cancel", [&, this]()
		{
			open = false;
		});
	}

	void NewSpritesheetModal::onComplete()
	{
		if (mTileWidth == 0 || mTileHeight == 0)
			return;

		mInProgressSheet = AssetManager::CreateMemoryOnlyAsset<Texture2DSheet>(mSheetName, mTexturePath, glm::vec2{ (f32)mTileWidth, (f32)mTileHeight });
	}
}