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
		void DrawMap();
		void DrawSheet();

		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		Ref<Tilemap> mTilemap;
		usize mMapWidth, mMapHeight;

		EditMode mEditMode = EditMode::Paint;
		bool mPainting = false;
		bool mDisplayColliders = true;

		usize mCurrentLayer = 0;
		Ref<Texture2DSheet> mCurrentSheet;
		Ref<SubTexture2D> mCurrentSubTex = nullptr;

		TileRenderData mCurrentTexTile;

		TileRenderData mHoveredTexTile;
	};

}
