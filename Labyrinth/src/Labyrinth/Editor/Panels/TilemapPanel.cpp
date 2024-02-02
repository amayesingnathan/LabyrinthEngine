#include "Lpch.h"
#include "TilemapPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/Modals/MapEditModal.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	void TilemapPanel::OnRender()
	{
		float cellSize = mThumbnailSize + mPadding;

		float panelWidth = Utils::AvailableRegion().x;
		i32 columnCount = (i32)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		f32 imageHeight = mThumbnailSize - Utils::LineHeight();
		glm::vec2 imageSize = imageHeight > 0 ? glm::vec2{ mThumbnailSize, imageHeight } : glm::vec2{ mThumbnailSize, mThumbnailSize };

		AssetHandle toDestroy = 0;
		Widgets::BeginColumns(columnCount);
		for (Ref<Tilemap> tilemap : AssetManager::GetAssetsWithType<Tilemap>())
		{
			LabWidgets::ImageButton(tilemap, imageSize, [&]() { ModalManager::Open<MapEditModal>("Edit Tilemap...", ModalButtons::OKCancel, tilemap); });
			Widgets::AddDragDropSource("TILEMAP_ITEM", tilemap->handle);

			Widgets::BeginContextPopup();
			Widgets::AddContextItem("Delete", [&]() { toDestroy = tilemap->handle; });
			Widgets::EndContextPopup();

			Widgets::NextColumn();
		}
		Widgets::EndColumns();

		if (toDestroy)
			AssetManager::DestroyAsset(toDestroy);

		Widgets::FloatEdit("Thumbnail Size", mThumbnailSize, 1.0f, 16, 512);
		Widgets::FloatEdit("Padding", mPadding, 1.0f, 0, 32);
	}
}

