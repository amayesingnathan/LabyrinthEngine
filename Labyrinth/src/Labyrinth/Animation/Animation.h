#pragma once
#include "AnimationFrame.h"

namespace Laby {

	class Animation : public Asset
	{
	public:
		ASSET_METADATA(AssetType::Animation, animations)

	public:
		Animation(std::string_view name) : mName(name) {}

		bool isPlaying() const { return mPlaying; }
		void play(bool start = true) { mPlaying = start; }
		bool step();
		void reset() { mFrameCounter = 0; mFrameIndex = 0; }

		AssetHandle currentFrame() const;

		std::string_view getName() const { return mName; }

		const std::vector<AnimationFrame>& getFrames() const { return mFrames; }
		void addFrame(AnimationFrame&& frame) { mFrames.push_back(std::move(frame)); }
		void addFrames(std::vector<AnimationFrame>&& frames) { mFrames = std::move(frames); }

	private:
		std::string mName;

		usize mFrameCounter = 0;
		std::vector<AnimationFrame> mFrames;
		usize mFrameIndex = 0;

		bool mPlaying = false;
	};
}