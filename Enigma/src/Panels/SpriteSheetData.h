#pragma once

#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Labyrinth {

	struct SpriteSheetData
	{
		std::string sheetName = "";
		std::string subTexName = "None";

		std::string texturePath;

		Ref<Texture2DSheet> currentSheet = nullptr;
		Ref<SubTexture2D> currentSubTex = nullptr;

		Ref<Framebuffer> framebuffer;
		glm::vec2 viewportSize = { 0.0f, 0.0f };
	};

}