#include "SubTexModal.h"

#include <Labyrinth/Assets/AssetManager.h>

namespace Labyrinth {

	SubTexModal::SubTexModal(SpriteSheetData& data) : Modal(), mPayload(data)
	{
		AssetHandle handle = AssetManager::GetAssetHandleFromPath(data.sheetName);
		mSheet = AssetManager::GetAsset<Texture2DSheet>(handle);
		mMaxWidthCount = mSheet->getTileCountX();
		mMaxHeightCount = mSheet->getTileCountY();

		mSquares.reserve(mMaxWidthCount * mMaxHeightCount);
		mPressedSquares.reserve(mMaxWidthCount * mMaxHeightCount);

		for (size_t i = 0; i < mMaxHeightCount; i++)
		{
			for (size_t j = 0; j < mMaxWidthCount; j++)
				mSquares.emplace_back(i, j);
		}
	}

	void SubTexModal::onImGuiRender()
	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		STR_COPY(buffer, mName);
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			mName = std::string(buffer);
		}

		float xpos = ImGui::GetCursorPosX();
		float ypos = ImGui::GetCursorPosY();

		auto imageSize = ImGui::GetWindowSize();
		imageSize = { imageSize.x - 2 * xpos, imageSize.y - 1.5f * ypos };
		ImVec2 tileSize = { imageSize.x / mMaxWidthCount, imageSize.y / mMaxHeightCount };
		ImGui::Image((ImTextureID)(uintptr_t)mSheet->getBaseTex()->getRendererID(), { imageSize.x, imageSize.y }, { 0, 1 }, { 1, 0 });

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
					auto eraseIt = std::find_if(mPressedSquares.begin(), mPressedSquares.end(), [&](SquareData& toDelete)
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
			{
				mPayload.subTexName = mName;
				mPayload.currentSubTex = mPayload.currentSheet->getSubTex(mPayload.subTexName);
				Close();
			}
			else
				LAB_WARN("Selection was not valid!");  //TODO: Add warning modal
			
			mWidthCount = 1; mHeightCount = 1;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			Close();
			mWidthCount = 1; mHeightCount = 1;
		}

	}

	bool SubTexModal::CheckSelection()
	{
		if (mPressedSquares.empty()) return false;

		SortSelected();

		//Get top leftmost selection and its index in the vector of all squares.
		const auto& firstSquare = mPressedSquares[0];
		size_t gridIndex = (firstSquare.pos.first * mMaxWidthCount) + firstSquare.pos.second;

		//Find the quad that extends from this selection to the right and down if there is one
		while (CheckRight(gridIndex))
		{
			gridIndex++;
			mWidthCount++;
		}
		while (CheckDown(gridIndex))
		{
			gridIndex += mMaxHeightCount;
			mHeightCount++;
		}

		//Check that we have the right amount of selected squares for this quad, we'll check if they're the correct ones later
		if ((mWidthCount * mHeightCount) != mPressedSquares.size()) return false;

		//Get the grid position of bottom right corner of this quad
		Position lastSquare = std::make_pair(firstSquare.pos.first + (mWidthCount - 1), firstSquare.pos.second + (mHeightCount - 1));
		
		// Check that all selections are within this quad
		for (const auto& selectedSquare : mPressedSquares)
		{
			if (selectedSquare == firstSquare) continue;
			if (selectedSquare == lastSquare) continue;

			if (!IsInSquare(firstSquare.pos, lastSquare, selectedSquare.pos)) return false;
		}

		float width = Cast<float>(lastSquare.first - firstSquare.pos.first + 1);
		float height = Cast<float>(lastSquare.second - firstSquare.pos.second + 1);

		mSheet->createSubTex(mName,
			{ Cast<float>(firstSquare.pos.second), Cast<float>(firstSquare.pos.first) }, 
			{ width, height });

		return true;
	}

	void SubTexModal::SortSelected()
	{
		std::sort(mPressedSquares.begin(), mPressedSquares.end(), [](const SquareData& x, const SquareData& y) 
			{
				return x.pos < y.pos;			
			});
	}

	bool SubTexModal::CheckRight(size_t gridIndex)
	{
		if ((gridIndex + 1) % mMaxWidthCount == 0) return false;

		return mSquares[gridIndex + 1].pressed;
	}

	bool SubTexModal::CheckDown(size_t gridIndex)
	{
		if (gridIndex + mMaxWidthCount >= mSquares.size()) return false;

		return  mSquares[gridIndex + mMaxWidthCount].pressed;
	}

	bool SubTexModal::IsInSquare(const Position& min, const Position& max, const Position& check)
	{
		return ((min <= check) && (check <= max));
	}

}