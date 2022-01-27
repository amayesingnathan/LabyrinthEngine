#pragma once

#include "../Modals/SubTexModal.h"

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Renderer/SubTexture.h"
#include "Labyrinth/Renderer/Framebuffer.h"

#include <imgui/imgui_internal.h>

namespace Labyrinth {

	class SpriteSheetPanel
	{
	public:
		SpriteSheetPanel();

		void onImGuiRender();

	private:
		void TileWidthModal();
		void SubTexModalRender();

	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Texture2D> mNoSheet;
		Ref<Texture2DSheet> mCurrentSheet;
		std::string mCurrentSheetPath;

		Ref<SubTexture2D> mSelectedSubTex;

		glm::vec2 mViewportSize = { 0.0f, 0.0f };

		SubTexModal subTexSelector;
		bool mSubTexModalOpen = false;

		uint32_t mSheetWidth = 0;
		uint32_t mSheetHeight = 0;
		int mTileWidth = 0;
		int mTileHeight = 0;

	};

}