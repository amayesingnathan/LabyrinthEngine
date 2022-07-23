#pragma once

#include "PanelManager.h"
#include "SpriteSheetData.h"

#include <Labyrinth/Renderer/Texture.h>

namespace Labyrinth {

	class SpriteSheetPanel : public IPanel
	{
	public:
		SpriteSheetPanel();

		void onUpdate() override;
		void onImGuiRender() override;

		static Ref<SpriteSheetPanel> Create() { return Ref<SpriteSheetPanel>::Create(); }

	private:
		Ref<Texture2D> mNoTex = nullptr;

		std::string mSheetName = "";

		SpriteSheetData mPanelData;

		size_t mSheetWidth = 0;
		size_t mSheetHeight = 0;
		i32 mTileWidth = 0;
		i32 mTileHeight = 0;

		const std::string noSubTex = "None";
	};

}