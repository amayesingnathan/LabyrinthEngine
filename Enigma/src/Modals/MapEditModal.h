#pragma once

#include <Labyrinth/Editor/EditorModal.h>
#include <Labyrinth/Tilemap/Tilemap.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	enum class EditMode
	{
		Paint, Behaviour
	};

	class MapEditModal : public EditorModal
	{
	public:
		MapEditModal(const Ref<Tilemap>& map);

		void onImGuiRender() override;
		void onComplete() override;
		void onEvent(Event& e) override;

		static Ref<MapEditModal> Create(const Ref<Tilemap>& map) { return Ref<MapEditModal>::Create(map); }

	private:
		void DrawMap();
		void DrawSheet();

		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		Ref<Tilemap> mTilemap;
		i32 mMapWidth, mMapHeight;

		EditMode mEditMode = EditMode::Paint;

		Ref<SubTexture2D> mCurrentSubTex = nullptr;

		usize mCurrentLayer = 0;
		TilePos mCurrentMapTile;
		i32 mCurrentTexTile = -1;
		SheetData mCurrentSheet;

		TilePos mHoveredMapTile;
		i32 mHoveredTexTile = -1;
	};

}
