#include "Lpch.h"
#include "TilemapPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/Modals/MapEditModal.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	void TilemapPanel::onImGuiRender()
	{
		float cellSize = mThumbnailSize + mPadding;

		float panelWidth = Utils::AvailableRegion().x;
		i32 columnCount = (i32)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		f32 imageHeight = mThumbnailSize - Utils::LineHeight();
		glm::vec2 imageSize = imageHeight > 0 ? glm::vec2{ mThumbnailSize, imageHeight } : glm::vec2{ mThumbnailSize, mThumbnailSize };

		Widgets::BeginColumns(columnCount);
		for (AssetHandle handle : AssetManager::GetAssetsWithType(AssetType::Tilemap))
		{
			Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(handle);

			Ref<IRenderable> tex = tilemap ? tilemap.to<IRenderable>() : EditorResources::NoTexture.to<IRenderable>();
			LabWidgets::ImageButton(tex, imageSize, [&]() { ModalManager::Open<MapEditModal>("Edit Tilemap...", ModalButtons::OKCancel, tilemap); });
			Widgets::AddDragDropSource("TILEMAP_ITEM", handle);

			Widgets::BeginContextPopup();
			Widgets::AddContextItem("Delete", [&]() {AssetManager::DestroyAsset(handle); });
			Widgets::EndContextPopup();
		}
		Widgets::EndColumns();

		Widgets::FloatEdit("Thumbnail Size", mThumbnailSize, 1.0f, 16, 512);
		Widgets::FloatEdit("Padding", mPadding, 1.0f, 0, 32);
	}
}

