#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Renderer/IRenderable.h>

#include "TilemapTexture.h"
#include "TileBehaviourLayer.h"

namespace Laby {

	class Tilemap : public IRenderable, public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::Tilemap)

	public:
		Tilemap(const std::string& name, u32 width, u32 height);

		const std::string& getName() const { return mName; }

		u32 getWidth() const { return mTexture.mWidth; }
		u32 getHeight() const { return mTexture.mHeight; }

		void addSheet(AssetHandle handle) { mTexture.addSheet(handle); }
		void addSheet(AssetHandle handle, TileID startIndex) { mTexture.addSheet(handle, startIndex); }
		void removeSheet(AssetHandle handle) { mTexture.removeSheet(handle); }

		Ref<SubTexture2D> getTileTex(TileID id) const { return mTexture.getTileTex(id); }

		TileRenderData getTileData(usize layer, const GridPosition& pos) const { return mTexture.getTile(layer, pos); }
		void setTileData(usize layer, const GridPosition& pos, const TileRenderData& renderData) { mTexture.setTile(layer, pos, renderData); }

		bool hasLayers() const { return mTexture.hasLayers(); }

		TileBehaviourData getTileBehaviour(const GridPosition& pos) const;
		void setTileBehaviour(const GridPosition& pos, const TileBehaviourData& behaviour) { mBehaviour(pos) = behaviour; }

		void addLayer() { mTexture.addLayer(); }
		void removeLayer(usize layer) { mTexture.removeLayer(layer); }
		void moveLayer(usize layer, LayerMoveDir direction) { mTexture.moveLayer(layer, direction); }

		u32 getTextureID() const override { return mTexture.mFramebuffer->getTextureID(); }
		void bindTexture(u32 slot = 0) const override { mTexture.mFramebuffer->bindTexture(slot); }

		std::vector<ChainShape> getPhysicsShapes() const { return mBehaviour.getShapes(); }
		std::vector<TileScriptData> getTileScripts() const { return mBehaviour.getScripts(); }

		void addLayer(const TileRenderLayer& layer) { mTexture.addLayer(layer); }
		void addLayer(TileRenderLayer&& layer) { mTexture.addLayer(std::move(layer)); }
		void setBehaviour(const TileBehaviourLayer& layer) { mBehaviour = layer; }
		void setBehaviour(TileBehaviourLayer&& layer) { mBehaviour = std::move(layer); }

		const std::vector<TileRenderLayer>& getLayers() const { return mTexture.getLayers(); }
		const std::vector<SheetData>& getSheets() const { return mTexture.getSheets(); }
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