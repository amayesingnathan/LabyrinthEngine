#pragma once

#include "PanelManager.h"
#include "SpriteSheetData.h"

#include "Labyrinth.h"

namespace Labyrinth {

	class SpriteSheetPanel : public IPanel
	{
	public:
		SpriteSheetPanel();

		void onUpdate() override;
		void onImGuiRender() override;

		static Ref<SpriteSheetPanel> Create() { return CreateRef<SpriteSheetPanel>(); }

	private:
		Ref<Texture2D> mNoTex = nullptr;

		std::string mSheetName = "";

		SpriteSheetData mPanelData;

		size_t mSheetWidth = 0;
		size_t mSheetHeight = 0;
		int mTileWidth = 0;
		int mTileHeight = 0;

		const std::string noSubTex = "None";
	};

}