#pragma once

#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	struct SpriteSheetData
	{
		Ref<Texture2DSheet> currentSheet = nullptr;
		Ref<SubTexture2D> currentSubTex = nullptr;
	};

}