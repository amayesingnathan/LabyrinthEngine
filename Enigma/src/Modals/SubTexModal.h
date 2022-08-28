#pragma once

#include "../Panels/SpriteSheetData.h"

#include <Labyrinth/Editor/EditorModal.h>
#include <Labyrinth/Renderer/SubTexture.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SubTexModal : public EditorModal
	{
		using Position = std::pair<size_t, size_t>;

	public:
		SubTexModal(SpriteSheetData& data);

		void onImGuiRender() override;
		void onComplete() override;

		static Ref<SubTexModal> Create(SpriteSheetData& data) { return Ref<SubTexModal>::Create(data); }

	private:
		//Verify this is a valid subtexture i.e. a quad
		bool CheckSelection();
		void SortSelected();

		//Navigate grid
		bool CheckRight(size_t gridIndex);
		bool CheckDown(size_t gridIndex);

		bool IsInSquare(const Position& min, const Position& max, const Position& check);

	private:
		struct SquareData
		{
			Position pos;
			bool pressed;

			SquareData() : pos(-1, -1), pressed(false) {}
			SquareData(const Position& p)
				: pos(p), pressed(false) {}
			SquareData(size_t x, size_t y)
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

		SpriteSheetData& mPayload;

		std::string mName = "SubTextureName";
		Ref<Texture2DSheet> mSheet = nullptr;

		Position outTopLeft;
		Position outBottomRight;

		std::vector<SquareData> mSquares = {};
		std::vector<SquareData> mPressedSquares = {};

		u32 mWidthCount = 1;
		u32 mHeightCount = 1;

		u32 mMaxWidthCount = 1;
		u32 mMaxHeightCount = 1;

		const ImVec4 mUnpressedColour = { 0.0f, 0.0f, 0.0f, 0.0f };
		const ImVec4 mPressedColour = { 0.25f, 1.0f, 1.0f, 0.5f };
	};

}