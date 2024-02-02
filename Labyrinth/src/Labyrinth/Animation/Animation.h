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
		ASSET_METADATA(AssetType::Animation, "animations")

	public:
		Animation(std::string_view name) : mName(name) {}

		std::string_view GetName() const { return mName; }

		const AnimationFrame& GetFrame(u32 frameIndex) const { return mFrames[frameIndex]; }
		std::vector<AnimationFrame>& GetFrames() { return mFrames; }
		const std::vector<AnimationFrame>& GetFrames() const { return mFrames; }
		usize GetFrameCount() const { return mFrames.size(); }
		bool HasFrames() const { return !mFrames.empty(); }

		void AddFrame(const AnimationFrame& frame) { mFrames.push_back(frame); }
		void AddFrames(std::vector<AnimationFrame>&& frames) { mFrames = std::move(frames); }

		void OverwriteFrame(const AnimationFrame& overwritingFrame);

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