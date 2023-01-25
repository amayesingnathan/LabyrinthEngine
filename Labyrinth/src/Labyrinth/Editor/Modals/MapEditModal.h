#pragma once

#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/Tilemap/Tilemap.h>

namespace Laby {

	enum class EditMode
	{
		Paint, Physics, Scripts
	};

	class MapEditModal : public IEditorModal
	{
	public:
		MapEditModal(const Ref<Tilemap>& map);

		void onImGuiRender() override;
		void onComplete() override;

		void onEvent(Event& e) override;
		LISTENING_EVENTS(KeyPressed)

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

		EditMode mEditMode = EditMode::Paint;
		bool mDisplayColliders = true;
		bool mCurrentlyPainting = false;

		usize mCurrentLayer = 0;
		AssetHandle mCurrentSheet;
		AssetHandle mSheetToAdd;

		GridPosition mCurrentTile;
		TileRenderData mCurrentTex;

		GridPosition mHoveredTile;
		TileRenderData mHoveredTex;
	};

}
