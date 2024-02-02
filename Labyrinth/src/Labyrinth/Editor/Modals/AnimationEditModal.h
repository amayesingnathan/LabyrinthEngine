#pragma once

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	enum class EditingMode { None, Add, Edit };

	class AnimationEditModal : public IEditorModal
	{
	public:
		AnimationEditModal(EditingMode mode, Ref<Animation> animation = nullptr);

		void OnRender() override;
		void OnCustomButtonRender(bool& open) override;
		void OnComplete() override;

	private:
		void StepAnimation();

	private:
		EditingMode mWindowMode;

		Ref<Animation> mAnimation;
		Ref<Texture2DSheet> mCurrentSheet;

		EditingMode mFrameMode = EditingMode::None;
		AnimationFrame mSelectedFrame;

		u32 mFrameCounter = 0;
		u32 mFrameIndex = 0;
		bool mPlaying = true;
	};
}