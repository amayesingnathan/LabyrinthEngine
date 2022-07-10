#pragma once

#include "Labyrinth.h"

namespace Labyrinth {

	enum class SheetAddType { Path, Texture };
	using SheetVariable = std::variant<std::string, Ref<Texture2D>>;

	struct SpriteSheetData
	{
		std::string sheetName = "";
		std::string subTexName = "None";

		SheetAddType addType = SheetAddType::Path;
		SheetVariable newSheetVar = std::string();

		Ref<Texture2DSheet> currentSheet = nullptr;
		Ref<SubTexture2D> currentSubTex = nullptr;

		Ref<Framebuffer> framebuffer;
		glm::vec2 viewportSize = { 0.0f, 0.0f };
	};

}