#pragma once

#include "PanelManager.h"

#include "Labyrinth.h"

namespace Labyrinth {

	class SubTexModal;

	struct SubTexPayload
	{
		std::string sheetName = "";
		std::string subTexName = "";
	};

	class SpriteSheetPanel : public Panel
	{
	public:
		enum class SheetAddType { Path, Texture };

	public:
		SpriteSheetPanel();

		void onUpdate() override;
		void onImGuiRender() override;

	private:
		void TileWidthModal();
		void SubTexModalRender();

	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Texture2D> mNoSheet = nullptr;
		Ref<Texture2DSheet> mCurrentSheet = nullptr;
		Ref<SubTexture2D> mCurrentSubTex = nullptr;

		SheetAddType mAddType = SheetAddType::Path;
		using SheetVariable = std::variant<std::string, Ref<Texture2D>>;
		SheetVariable mNewSheetVar = std::string();

		std::string mSheetName = "";

		glm::vec2 mViewportSize = { 0.0f, 0.0f };

		SubTexModal* mSubTexSelector = nullptr;
		SubTexPayload mPayload;


		uint32_t mSheetWidth = 0;
		uint32_t mSheetHeight = 0;
		int mTileWidth = 0;
		int mTileHeight = 0;

		const std::string noSubTex = "None";
	};

}