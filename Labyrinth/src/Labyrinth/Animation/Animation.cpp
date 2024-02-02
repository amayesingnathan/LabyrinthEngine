#include "Lpch.h"
#include "Animation.h"

namespace Laby {

	void Animation::OverwriteFrame(const AnimationFrame& overwritingFrame)
	{
		auto it = std::ranges::find_if(mFrames, [&](const auto& frame) { return frame.id == overwritingFrame.id; });
		if (it == mFrames.end())
			return;

		*it = overwritingFrame;
	}

	Ref<Animation> AnimationUtils::Clone(Ref<Animation> other)
	{
		Ref<Animation> copy = Ref<Animation>::Create(other->mName);
		copy->mFrames = other->mFrames;
		copy->handle = other->handle;

		return copy;
	}
}
