#pragma once

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Renderer/IRenderable.h>

#include "TilemapTexture.h"
#include "TileBehaviourLayer.h"

namespace Laby {

	class Tilemap : public IRenderable, public Asset
	{
	public:
		ASSET_METADATA(AssetType::Tilemap, "tilemaps")

	public:
		Tilemap(const std::string& name, u32 width, u32 height);

		const std::string& GetName() const { return mName; }

		u32 GetWidth() const { return mTexture.mWidth; }
		u32 GetHeight() const { return mTexture.mHeight; }

		void AddSheet(AssetHandle handle) { mTexture.AddSheet(handle); }
		void AddSheet(AssetHandle handle, TileID startIndex) { mTexture.AddSheet(handle, startIndex); }
		void RemoveSheet(AssetHandle handle) { mTexture.RemoveSheet(handle); }

		Ref<SubTexture2D> GetTileTex(TileID id) const { return mTexture.GetTileTex(id); }

		TileRenderData GetTileData(usize layer, const GridPosition& pos) const { return mTexture.GetTile(layer, pos); }
		void SetTileData(usize layer, const GridPosition& pos, const TileRenderData& renderData) { mTexture.SetTile(layer, pos, renderData); }

		bool HasLayers() const { return mTexture.HasLayers(); }

		TileBehaviourData GetTileBehaviour(const GridPosition& pos) const;
		void SetTileBehaviour(const GridPosition& pos, const TileBehaviourData& behaviour) { mBehaviour(pos) = behaviour; }

		void AddLayer() { mTexture.AddLayer(); }
		void RemoveLayer(usize layer) { mTexture.RemoveLayer(layer); }
		void MoveLayer(usize layer, LayerMoveDir direction) { mTexture.MoveLayer(layer, direction); }

		u32 GetTextureID() const override { return mTexture.mFramebuffer->GetTextureID(); }

		std::vector<ChainShape> GetPhysicsShapes() const { return mBehaviour.GetShapes(); }
		std::vector<TileScriptData> GetTileScripts() const { return mBehaviour.GetScripts(); }

		void AddLayer(const TileRenderLayer& layer) { mTexture.AddLayer(layer); }
		void AddLayer(TileRenderLayer&& layer) { mTexture.AddLayer(std::move(layer)); }
		void SetBehaviour(const TileBehaviourLayer& layer) { mBehaviour = layer; }
		void SetBehaviour(TileBehaviourLayer&& layer) { mBehaviour = std::move(layer); }

		const std::vector<TileRenderLayer>& GetLayers() const { return mTexture.GetLayers(); }
		const std::vector<SheetData>& GetSheets() const { return mTexture.GetSheets(); }
		const TileBehaviourLayer& GetBehaviour() const { return mBehaviour; }

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