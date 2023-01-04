#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Renderer/IRenderable.h>

#include "TilemapTexture.h"
#include "TileBehaviourLayer.h"

namespace Laby {

	class Tilemap : public IRenderable, public Asset
	{
	public:
		Tilemap(const std::string& name, usize width, usize height);

		const std::string& getName() const { return mName; }

		usize getWidth() const { return mTexture.mWidth; }
		usize getHeight() const { return mTexture.mHeight; }

		u32 getTextureID() const override { return mTexture.mFramebuffer->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { mTexture.mFramebuffer->bindTexture(slot); }

		std::vector<ChainShape> getPhysicsShapes() const { return mBehaviour.getShapes(); }

	private:
		std::string mName;
		TilemapTexture mTexture;
		TileBehaviourLayer mBehaviour;
	};
}