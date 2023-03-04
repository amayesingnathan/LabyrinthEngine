#include "Lpch.h"
#include "AnimationPanel.h"

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/Modals/AnimationEditModal.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/SubTexture.h>

using imcpp::Utils;
using imcpp::Widgets;

namespace Laby {

	void AnimationPanel::onImGuiRender()
	{
		f32 cellSize = mThumbnailSize + mPadding;
		f32 panelWidth = Utils::AvailableRegion().x;

		i32 columnCount = (i32)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		f32 imageHeight = mThumbnailSize - Utils::LineHeight();
		glm::vec2 imageSize = imageHeight > 0 ? glm::vec2{ mThumbnailSize, imageHeight } : glm::vec2{ mThumbnailSize, mThumbnailSize };

		AssetHandle toDestroy = 0;
		Widgets::BeginColumns(columnCount);

		for (Ref<Animation> animation : AssetManager::GetAssetsWithType<Animation>())
		{
			LabWidgets::ImageButton(AssetManager::GetAsset<SubTexture2D>(animation->currentFrame()),
				imageSize, [=]() { ModalManager::Open<AnimationEditModal>("Edit Animation...", ModalButtons::Custom, EditingMode::Edit, animation); });
			Widgets::AddDragDropSource("ANIMATION_ITEM", animation->handle);

			Widgets::BeginContextPopup();
			Widgets::AddContextItem("Delete", [&]() { toDestroy = animation->handle; });
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