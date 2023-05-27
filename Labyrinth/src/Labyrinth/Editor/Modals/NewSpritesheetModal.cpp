#include "Lpch.h"
#include "NewSpritesheetModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

#include "SubTexturePickerModal.h"

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	NewSpritesheetModal::NewSpritesheetModal(Ref<Texture2DSheet>& context, const fs::path& filepath)
		: mCurrentSheet(context), mTexturePath(filepath)
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
			mCurrentSheet->generateTileset();
			open = false;
		});

		Widgets::SameLine();

		Widgets::Button("Pick", [&, this]()
		{
			onComplete();
			ModalManager::Open<SubTexturePickerModal>("Select subtextures...", ModalButtons::OKCancel, mCurrentSheet);
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

		mCurrentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mSheetName, mSheetName, mTexturePath, glm::vec2{ (f32)mTileWidth, (f32)mTileHeight });
	}
}