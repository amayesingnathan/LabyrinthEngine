#pragma once

#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/Tilemap/Tilemap.h>

namespace Laby {

	class MapEditModal : public IEditorModal
	{
	public:
		MapEditModal(const Ref<Tilemap>& map);

		void onImGuiRender() override;
		void onComplete() override;

	private:
		void LeftPane();
		void CentrePane();
		void RightPane();

		void DrawSheet();

		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		Ref<Tilemap> mTilemap;
		u32 mMapWidth, mMapHeight;

		f32 mFrameHeightWithSpacing = 0.0f;

		enum class EditMode { Paint, Selection };
		EditMode mEditMode = EditMode::Selection;

		bool mDisplayColliders = true;
		bool mCurrentlyPainting = false;

		usize mCurrentLayer = 0;
		AssetHandle mCurrentSheet;
		AssetHandle mSheetToAdd;

		TileRenderData mBrushSubtex;

		GridPosition mCurrentTile;
		TileRenderData mCurrentSubtex;
		TileBehaviourData mCurrentBehaviour;

		GridPosition mHoveredTile;
		TileRenderData mHoveredSubtex;
		TileBehaviourData mHoveredBehaviour;
	};

}
