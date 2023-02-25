#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>

namespace Laby {

	class AnimationPanel : public IEditorPanel
	{
	public:
		AnimationPanel() = default;

		void onImGuiRender() override;

	private:
		f32 mPadding = 16.0f;
		f32 mThumbnailSize = 128.0f;
	};
}