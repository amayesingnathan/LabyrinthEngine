#include "SubTexModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>

namespace Labyrinth {

	SubTexModal::SubTexModal(SpriteSheetData& data) : EditorModal(), mPayload(data)
	{
		mSheet = mPayload.currentSheet;
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
		StaticString<256> buffer(mName);
		if (ImGui::InputText("Name", buffer, buffer.length()))
			mName = buffer.toString();

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
				ImGui::PushStyleColor(ImGuiCol_Button, EditorResources::HoveredColour);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, EditorResources::ClearColour);
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
	}

	void SubTexModal::onComplete()
	{
		if (CheckSelection())
		{
			mPayload.subTexName = mName;
			mPayload.currentSubTex = mPayload.currentSheet->getSubTex((i32)std::hash<std::string>()(mName));
		}
		else
			ModalManager::OpenWarning("Invalid selection", "Selection was not valid!");
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

		f32 width = (f32)(lastSquare.first - firstSquare.pos.first + 1);
		f32 height = (f32)(lastSquare.second - firstSquare.pos.second + 1);

		mSheet->createSubTex((i32)std::hash<std::string>()(mName), mName,
			{ (f32)firstSquare.pos.second, (f32)firstSquare.pos.first },
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