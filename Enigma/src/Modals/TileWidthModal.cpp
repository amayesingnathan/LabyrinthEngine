#include "TileWidthModal.h"

#include <Labyrinth/Core/Buffer.h>

namespace Labyrinth {

	TileWidthModal::TileWidthModal(SpriteSheetData& data) : Modal(), mPayload(data)
	{
	}

	void TileWidthModal::onImGuiRender()
	{
		ImGui::Text("Please enter the width and height of each tile in the sprite sheet:");
		ImGui::NewLine();

		StaticBuffer<256> buffer(mPayload.sheetName);
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			mPayload.sheetName = buffer.string();

		ImGui::InputInt("Width", &mTileWidth);
		ImGui::InputInt("Height", &mTileHeight);
		ImGui::NewLine();

		bool loadSheet = false;

		if (ImGui::Button("OK"))
		{
			if (mTileWidth > 0 && mTileHeight > 0)
				LoadSheet();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			mPayload.addType = SheetAddType::Path;
			mPayload.newSheetVar = std::string();
			Close();
		}

	}

	void TileWidthModal::LoadSheet()
	{
		switch (mPayload.addType)
		{
			case SheetAddType::Path:	mPayload.currentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mPayload.sheetName + ".lss", "assets/spritesheets/" + mPayload.sheetName, std::get<std::string>(mPayload.newSheetVar), glm::vec2{mTileWidth, mTileHeight}, mPayload.sheetName); break;
			case SheetAddType::Texture: mPayload.currentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mPayload.sheetName + ".lss", "assets/spritesheets/" + mPayload.sheetName, std::get<Ref<Texture2D>>(mPayload.newSheetVar), glm::vec2{mTileWidth, mTileHeight}, mPayload.sheetName); break;
		}

		mPayload.framebuffer->resize(Cast<size_t>(mPayload.viewportSize.x) - 15, 200);

		Close();
	}

}