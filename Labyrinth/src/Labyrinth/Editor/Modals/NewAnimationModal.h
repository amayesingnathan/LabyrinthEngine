#pragma once

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class NewAnimationModal : public IEditorModal
	{
	public:
		NewAnimationModal() = default;

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		Ref<Texture2DSheet> mSheet;

		std::string mAnimationName;

		std::vector<AnimationFrame> mNewAnimation;

		AnimationFrame mFrameInProgress;
		AnimationFrame* mSelectedFrame = nullptr;

	};
}