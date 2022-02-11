#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SubTexModal
	{
		using Position = std::pair<int, int>;

	public:
		SubTexModal() = default;

		void display(Ref<Texture2DSheet> sheet, struct SubTexPayload& data);

	private:
		void Close();

		//Verify this is a valid subtexture i.e. a quad
		bool CheckSelection(Ref<Texture2DSheet> sheet);
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

		bool open = false;

		std::string mName = "SubTextureName";

		Position outTopLeft;
		Position outBottomRight;

		std::vector<SquareData> mSquares = {};
		std::vector<SquareData> mPressedSquares = {};

		uint32_t mWidthCount = 1;
		uint32_t mHeightCount = 1;

		uint32_t mMaxWidthCount = 1;
		uint32_t mMaxHeightCount = 1;

		const ImVec4 mUnpressedColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		const ImVec4 mPressedColour = { 0.25f, 1.0f, 1.0f, 0.5f };
	};

}