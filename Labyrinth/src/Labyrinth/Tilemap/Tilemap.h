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

		void addSheet(AssetHandle handle) { mTexture.addSheet(handle); }
		void addLayer() { mTexture.addLayer(); }

		u32 getTextureID() const override { return mTexture.mFramebuffer->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { mTexture.mFramebuffer->bindTexture(slot); }

		std::vector<ChainShape> getPhysicsShapes() const { return mBehaviour.getShapes(); }
		std::vector<TileScriptData> getTileScripts() const { return mBehaviour.getScripts(); }

	private:
		void addLayer(const TileRenderLayer& layer) { mTexture.addLayer(layer); }
		void addLayer(TileRenderLayer&& layer) { mTexture.addLayer(std::move(layer)); }
		void setBehaviour(const TileBehaviourLayer& layer) { mBehaviour = layer; }
		void setBehaviour(TileBehaviourLayer&& layer) { mBehaviour = std::move(layer); }

		const std::vector<TileRenderLayer>& getLayers() const { return mTexture.getLayers(); }
		const std::unordered_set<AssetHandle>& getSheets() const { return mTexture.getSheets(); }
		const TileBehaviourLayer& getBehaviour() const { return mBehaviour; }

	private:
		std::string mName;
		TilemapTexture mTexture;
		TileBehaviourLayer mBehaviour;

		friend class TilemapSerialiser;
		friend class TilemapUtils;
	};

	class TilemapUtils
	{
	public:
		static Ref<Tilemap> Clone(Ref<Tilemap> other);
	};
}