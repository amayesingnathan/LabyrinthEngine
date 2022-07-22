#pragma once

#include "Modal.h"
#include "../Panels/SpriteSheetData.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class TileWidthModal : public Modal
	{
	public:
		TileWidthModal(SpriteSheetData& data);

		void onImGuiRender() override;

		static Ref<TileWidthModal> Create(SpriteSheetData& data) { return CreateRef<TileWidthModal>(data); }

	private:
		void LoadSheet();

	private:
		SpriteSheetData& mPayload;

		size_t mSheetWidth = 0;
		size_t mSheetHeight = 0;
		i32 mTileWidth = 0;
		i32 mTileHeight = 0;

	};

}