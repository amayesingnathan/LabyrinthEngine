#include "TileWidthModal.h"

#include <Labyrinth/Core/Buffer.h>
#include <Labyrinth/Assets/AssetManager.h>

namespace Labyrinth {

	TileWidthModal::TileWidthModal(SpriteSheetData& data) : EditorModal(), mPayload(data)
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

	}

	void TileWidthModal::onComplete()
	{
		if (mTileWidth > 0 && mTileHeight > 0)
		{
			mPayload.currentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mPayload.sheetName + ".lss", "spritesheets/" + mPayload.sheetName, mPayload.texturePath, glm::vec2{ mTileWidth, mTileHeight }, mPayload.sheetName);
			mPayload.framebuffer->resize(Cast<size_t>(mPayload.viewportSize.x) - 15, 200);
		}
	}


}