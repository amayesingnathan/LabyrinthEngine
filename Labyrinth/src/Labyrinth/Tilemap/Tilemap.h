#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Renderer/IRenderable.h>

#include "TilemapTexture.h"

namespace Laby {

	class Tilemap : public IRenderable, public Asset
	{
	public:
		u32 getTextureID() const override { return mTexture.mFramebuffer->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { mTexture.mFramebuffer->bindTexture(slot); }

	private:
		TilemapTexture mTexture;
	};
}