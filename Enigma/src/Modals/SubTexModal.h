#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SubTexModal
	{
	public:
		SubTexModal() = default;

		void init(Ref<Texture2DSheet> sheet);
		void display();

	private:
		void Close();

	private:
		Ref<Texture2DSheet> mSheet;
		std::map<std::pair<int, int>, bool> mSquares = {};

		const ImVec4 mUnpressedColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		const ImVec4 mPressedColour = { 0.25f, 1.0f, 1.0f, 0.5f };
	};

}