#include "Lpch.h"
#include "Animation.h"

namespace Laby {

	bool Animation::step()
	{
		if (!mPlaying)
			return false;

		mFrameCounter++;
		if (mFrameCounter < mFrames[mFrameIndex].frameLength)
			return false;

		++mFrameIndex %= mFrames.size();
		mFrameCounter = 0;
		return true;
	}

	AssetHandle Animation::currentFrame() const
	{
		if (mFrameIndex >= mFrames.size())
			return 0;

		return mFrames[mFrameIndex].sprite;
	}

	Ref<Animation> AnimationUtils::Clone(Ref<Animation> other)
	{
		Ref<Animation> copy = Ref<Animation>::Create(other->mName);
		copy->mFrames = other->mFrames;
		copy->handle = other->handle;

		return copy;
	}
}
