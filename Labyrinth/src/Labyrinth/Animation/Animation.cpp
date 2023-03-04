#include "Lpch.h"
#include "Animation.h"

namespace Laby {

	bool Animation::step()
	{
		if (!mPlaying)
			return false;

		mFrameCounter++;
		if (mFrameCounter < mFrames[mFrameIndex].length)
			return false;

		mFrameIndex++;
		if (mFrameIndex == mFrames.size())
		{
			mPlaying = !mPlayOnce;
			mFrameIndex = 0;
		}
		mFrameCounter = 0;
		return true;
	}

	AssetHandle Animation::currentFrame() const
	{
		if (mFrameIndex >= mFrames.size())
			return 0;

		return mFrames[mFrameIndex].sprite;
	}

	void Animation::overwriteFrame(const AnimationFrame& overwritingFrame)
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
