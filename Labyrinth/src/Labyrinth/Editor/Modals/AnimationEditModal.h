#pragma once

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Editor/IEditorModal.h>

namespace Laby {

	class AnimationEditModal : public IEditorModal
	{
	public:
		AnimationEditModal(Ref<Animation> animation);

		void onImGuiRender() override;
		void onComplete() override;

	private:
		Ref<Animation> mAnimation;
	};
}