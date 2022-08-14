#pragma once

#include "Tilemap.h"

namespace Labyrinth {

	class TilemapManager
	{
	public:
		Ref<Tilemap> GetTilemap() { return sActiveTilemap; }
		void SetTilemap(const Ref<Tilemap>& tilemap) { sActiveTilemap = tilemap; }

		static void SetContext(const Ref<Scene>& scene)
		{
			LAB_CORE_ASSERT(sActiveTilemap, "No active tilemap!");
			sActiveTilemap->mContext = scene;
		}

		static void RegenTexture()
		{
			LAB_CORE_ASSERT(sActiveTilemap, "No active tilemap!");
			sActiveTilemap->RegenTexture();
		}

	private:
		inline static Ref<Tilemap> sActiveTilemap = nullptr;
	};

}