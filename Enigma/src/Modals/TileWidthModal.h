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

		static Ref<TileWidthModal> Create(SpriteSheetData& data) { return Ref<TileWidthModal>::Create(data); }

	private:
		void LoadSheet();

	private:
		SpriteSheetData& mPayload;

		i32 mTileWidth = 0;
		i32 mTileHeight = 0;

	};

}