#pragma once

#include "../Panels/SpriteSheetData.h"

#include <Labyrinth/Editor/EditorModal.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class TileWidthModal : public EditorModal
	{
	public:
		TileWidthModal(SpriteSheetData& data);

		void onImGuiRender() override;
		void onComplete() override;

		static Ref<TileWidthModal> Create(SpriteSheetData& data) { return Ref<TileWidthModal>::Create(data); }

	private:
		SpriteSheetData& mPayload;

		i32 mTileWidth = 0;
		i32 mTileHeight = 0;

	};

}