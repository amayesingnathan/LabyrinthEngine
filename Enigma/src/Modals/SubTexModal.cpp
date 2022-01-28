#include "SubTexModal.h"

namespace Labyrinth {

	void SubTexModal::init(Ref<Texture2DSheet> sheet)
	{
		mName = "";

		mWidthCount = 1;
		mHeightCount = 1;

		mMaxWidthCount = sheet->getTileCountX();
		mMaxHeightCount = sheet->getTileCountY();

		mSquares.reserve(mMaxWidthCount * mMaxHeightCount);
		mPressedSquares.reserve(mMaxWidthCount * mMaxHeightCount);

		for (int i = 0; i < mMaxHeightCount; i++)
		{
			for (int j = 0; j < mMaxWidthCount; j++)
				mSquares.emplace_back(i, j);
		}

		mSheet = sheet;

	}

	void SubTexModal::display()
	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			mName = std::string(buffer);
		}

		float xpos = ImGui::GetCursorPosX();
		float ypos = ImGui::GetCursorPosY();

		auto imageSize = ImGui::GetWindowSize();
		imageSize = { imageSize.x - 2 * xpos, imageSize.y - 1.5f * ypos };
		ImVec2 tileSize = { imageSize.x / mMaxWidthCount, imageSize.y / mMaxHeightCount };
		ImGui::Image((void*)mSheet->getTex()->getRendererID(), { imageSize.x, imageSize.y }, { 0, 1 }, { 1, 0 });

		const ImVec4 onColour = {};

		auto& colours = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
		const auto& buttonActive = colours[ImGuiCol_ButtonActive];

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		for (auto& square : mSquares)
		{
			std::string name = "##Button" + std::to_string(square.pos.first) + std::to_string(square.pos.second);

			ImGui::SetCursorPosX(xpos + (square.pos.second * tileSize.x));
			ImGui::SetCursorPosY(ypos + (square.pos.first * tileSize.y));

			if (square.pressed)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, mPressedColour);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, mUnpressedColour);
			}

			if (ImGui::Button(name.c_str(), tileSize))
			{
				square.pressed = !square.pressed;
				if (square.pressed)
					mPressedSquares.emplace_back(square);
				else
				{
					auto& eraseIt = std::find_if(mPressedSquares.begin(), mPressedSquares.end(), [&](SquareData& toDelete)
						{
							return square.pos == toDelete.pos;
						});
					if (eraseIt != mPressedSquares.end()) mPressedSquares.erase(eraseIt);
				}
			}

			ImGui::PopStyleColor();

		}

		ImGui::PopStyleColor(2);

		if (ImGui::Button("OK"))
		{
			if (CheckSelection())
				Close();
			else
				LAB_WARN("Selection was not valid!");
			
			mWidthCount = 1; mHeightCount = 1;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			Close();
		}

	}

	void SubTexModal::Close()
	{
		ImGui::CloseCurrentPopup();
	}

	bool SubTexModal::CheckSelection()
	{
		if (mPressedSquares.empty()) return false;

		SortSelected();

		//Get top leftmost selection.
		const auto& firstSquare = mPressedSquares[0];
		int selectedGridIndex = (firstSquare.pos.first * mMaxWidthCount) + firstSquare.pos.second;

		//Find the quad that extends from this selection to the right and down if there is one
		int checkGridIndex = selectedGridIndex;
		while (CheckRight(checkGridIndex))
		{
			checkGridIndex++;
			mWidthCount++;
		}
		while (CheckDown(checkGridIndex))
		{
			checkGridIndex += mMaxHeightCount;
			mHeightCount++;
		}

		//Check that we have the right amount of selected squares for this quad, we'll check if they're the correct ones later
		if ((mWidthCount * mHeightCount) != mPressedSquares.size()) return false;

		//Get the grid position of bottom right corner of this quad
		Position lastSquare = std::make_pair(firstSquare.pos.first + mWidthCount, firstSquare.pos.second + mHeightCount);
		
		// Check that all selections are within this quad
		for (const auto& selectedSquare : mPressedSquares)
		{
			if (selectedSquare == firstSquare) continue;
			if (selectedSquare == lastSquare) continue;

			if (!IsInSquare(firstSquare.pos, lastSquare, selectedSquare.pos)) return false;
		}

		int width = lastSquare.first - firstSquare.pos.first;
		int height = lastSquare.second - firstSquare.pos.second;

		mSheet->createSubTex(mName,
			{ Cast<float>(firstSquare.pos.first), Cast<float>(firstSquare.pos.second) }, 
			{ Cast<float>(width), Cast<float>(height) });

		return true;
	}

	void SubTexModal::SortSelected()
	{
		std::sort(mPressedSquares.begin(), mPressedSquares.end(), [](const SquareData& x, const SquareData& y) 
			{
				return x.pos < y.pos;			
			});
	}

	bool SubTexModal::CheckRight(int gridIndex)
	{
		if ((gridIndex + 1) % mMaxWidthCount == 0) return false;

		return mSquares[gridIndex + 1].pressed;
	}

	bool SubTexModal::CheckDown(int gridIndex)
	{
		if (gridIndex + mMaxWidthCount >= mSquares.size()) return false;

		return  mSquares[gridIndex + mMaxWidthCount].pressed;
	}

	bool SubTexModal::IsInSquare(const Position& min, const Position& max, const Position& check)
	{
		return ((min <= check) && (check <= max));
	}

}