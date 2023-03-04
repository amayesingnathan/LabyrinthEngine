#pragma once
#include "AnimationFrame.h"

namespace Laby {

	enum class AnimationState
	{
		NoChange, NewFrame, CycleComplete
	};

	class Animation : public Asset
	{
	public:
		ASSET_METADATA(AssetType::Animation, animations)

	public:
		Animation(std::string_view name) : mName(name) {}

		std::string_view getName() const { return mName; }

		const AnimationFrame& getFrame(u32 frameIndex) const { return mFrames[frameIndex]; }
		std::vector<AnimationFrame>& getFrames() { return mFrames; }
		const std::vector<AnimationFrame>& getFrames() const { return mFrames; }
		usize getFrameCount() const { return mFrames.size(); }
		bool hasFrames() const { return !mFrames.empty(); }

		void addFrame(const AnimationFrame& frame) { mFrames.push_back(frame); }
		void addFrames(std::vector<AnimationFrame>&& frames) { mFrames = std::move(frames); }

		void overwriteFrame(const AnimationFrame& overwritingFrame);

		auto begin() { return mFrames.begin(); }
		auto begin() const { return mFrames.cbegin(); }
		auto end() { return mFrames.end(); }
		auto end() const { return mFrames.cend(); }

	private:
		std::string mName;
		std::vector<AnimationFrame> mFrames;

		friend class AnimationUtils;
		friend class AnimationEditModal;
	};

	class AnimationUtils
	{
	public:
		static Ref<Animation> Clone(Ref<Animation> other);
	};
}