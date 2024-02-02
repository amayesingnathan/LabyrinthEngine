#pragma once

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/IEditorPanel.h>

namespace Laby {

	class TilemapPanel : public IEditorPanel
	{
	public:
		TilemapPanel() = default;

		void OnRender() override;

	private:
		AssetHandle mSelected = 0;

		f32 mPadding = 16.0f;
		f32 mThumbnailSize = 128.0f;
	};

}