#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SubTexModal
	{
		using Position = std::pair<int, int>;

	public:
		SubTexModal() = default;

		void init(Ref<Texture2DSheet> sheet);
		void display();

	private:
		void Close();

		//Verify this is a valid subtexture i.e. a quad
		bool CheckSelection();
		void SortSelected();

		//Navigate grid
		bool CheckRight(int gridIndex);
		bool CheckDown(int gridIndex);

		bool IsInSquare(const Position& min, const Position& max, const Position& check);

	private:
		struct SquareData
		{
			Position pos;
			bool pressed;

			SquareData() : pos(-1, -1), pressed(false) {}
			SquareData(const Position& p)
				: pos(p), pressed(false) {}
			SquareData(int x, int y)
				: pos(x, y), pressed(false) {}

			bool operator== (const SquareData& other) const
			{
				return pos == other.pos;
			}
			bool operator== (const Position& other) const
			{
				return pos == other;
			}
		};

		bool success = false;

		std::string mName;

		Position outTopLeft;
		Position outBottomRight;

		Ref<Texture2DSheet> mSheet;

		std::vector<SquareData> mSquares = {};
		std::vector<SquareData> mPressedSquares = {};

		uint32_t mWidthCount = 0;
		uint32_t mHeightCount = 0;

		uint32_t mMaxWidthCount;
		uint32_t mMaxHeightCount;

		const ImVec4 mUnpressedColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		const ImVec4 mPressedColour = { 0.25f, 1.0f, 1.0f, 0.5f };
	};

}