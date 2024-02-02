#include "Lpch.h"
#include "SubTexturePickerModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	SubTexturePickerModal::SubTexturePickerModal(AssetHandle& returnSheet, AssetHandle sheetInProgress)
		: mReturnSheet(&returnSheet),
		  mCurrentSheet(AssetManager::GetAsset<Texture2DSheet>(sheetInProgress)),
		  mTextureSelections(mCurrentSheet->GetTileCountX(), mCurrentSheet->GetTileCountY())
	{
	}

	void SubTexturePickerModal::OnRender()
	{
		const glm::vec2& region = Utils::AvailableRegion<glm::vec2>();
		glm::vec2 sheetRegion = { 0.75f * region.x, region.y };
		Widgets::BeginChild("SpriteSheetDisplay", sheetRegion);

		glm::vec2 cursorPos = Utils::CursorPos<glm::vec2>();

		LabWidgets::Image(mCurrentSheet, sheetRegion);

		if (mCurrentSheet)
		{
			glm::vec2 tempCursorPos = Utils::CursorPos<glm::vec2>();
			u32 tileCountX = mCurrentSheet->GetTileCountX();
			u32 tileCountY = mCurrentSheet->GetTileCountY();

			Utils::SetButtonDefaults();
			Widgets::GridControl<glm::vec2>(cursorPos, sheetRegion, tileCountX, tileCountY, [=](u32 x, u32 y, const glm::vec2& tileSize)
			{
				GridPosition pos(x, tileCountY - y - 1);
				bool selected = mTextureSelections(pos);
				glm::vec4 buttonColour = selected ? glm::vec4{1.0f, 0.82f, 0.0f, 0.5f} : Colour::Transparent;

				Utils::SetButtonColour(buttonColour);
				Widgets::Button(tileSize, std::format("##{}({}, {})", "SheetPicker", pos.x, pos.y), [=]()
				{
					mTextureSelections(pos) = !selected;
					if (mTextureSelections(pos))
						mCurrentlyAdding.points.push_back(pos);
					else
						std::erase(mCurrentlyAdding.points, pos);
				});
				Utils::PopStyleColour(1);
			});
			Utils::PopStyleColour(2);
			Utils::SetCursorPos(tempCursorPos.x, tempCursorPos.y);
		}

		Widgets::EndChild();

		Widgets::SameLine();

		Widgets::BeginChild("SubTexturesAdded");

		const glm::vec2& listRegion = Utils::AvailableRegion<glm::vec2>();
		glm::vec2 subtexImageSize = { 0.25f * listRegion.x, 0.1f * listRegion.y };

		AssetHandle toRemove = 0;
		for (AssetHandle tex : mCurrentSheet->GetSubTextures())
		{
			LabWidgets::Image(AssetManager::GetAsset<SubTexture2D>(tex), subtexImageSize);

			Widgets::SameLine();

			Widgets::Button("Remove", [&toRemove, tex] { toRemove = tex; });
		}

		if (toRemove)
			mCurrentSheet->DestroySubTex(toRemove);

		Widgets::EndChild();
	}

	void SubTexturePickerModal::OnCustomButtonRender(bool& open)
	{
		Widgets::Button("Add", [&, this]()
		{
			if (!mCurrentlyAdding.valid())
			{
				mErrorText = "This selection is not valid!";
				return;
			}

			mErrorText = std::string();
			AssetHandle subtex = mCurrentSheet->CreateSubTex(mCurrentSheet->SubTexCount(), mCurrentlyAdding.pos(), mCurrentlyAdding.size());

			mCurrentlyAdding.points.clear();
			mTextureSelections.Reset();
		});

		Widgets::SameLine();

		Widgets::Button("Create Selected", [&, this]()
		{
			OnComplete();
			open = false;
		});

		Widgets::SameLine();

		Widgets::Button("Cancel", [&, this]()
		{
			open = false;
		});

		Widgets::SameLine();

		Widgets::Label(mErrorText);
	}

	void SubTexturePickerModal::OnComplete()
	{
		std::string_view name = mCurrentSheet->GetName();
		mCurrentSheet = AssetManager::SaveMemoryOnlyAsset<Texture2DSheet>(name, mCurrentSheet->handle);
		*mReturnSheet = mCurrentSheet->handle;

		usize index = 0;
		for (AssetHandle tex : mCurrentSheet->GetSubTextures())
			AssetManager::SaveMemoryOnlyAsset<SubTexture2D>(std::format("{}/{}", name, index++), tex);
	}

	bool SubTexturePickerModal::NewTexture::valid() const
	{
		GridPosition bottomLeft = { MinX(), MinY() };
		GridPosition topRight = { MaxX(), MaxY() };

		if (!bottomLeft.Valid() || !topRight.Valid())
			return false;

		// All points must be within rectangle
		for (const GridPosition& pos : points)
		{
			if (pos.x < bottomLeft.x)
				return false;

			if (pos.x > topRight.x)
				return false;

			if (pos.y < bottomLeft.y)
				return false;

			if (pos.y > topRight.y)
				return false;
		}

		// If all points are within rectangle and number of points is equal to width * height, then this is a rectangle
		u32 expectedCount = (topRight.x - bottomLeft.x + 1) * (topRight.y - bottomLeft.y + 1);
		return expectedCount == points.size();
	}

	u32 SubTexturePickerModal::NewTexture::MinX() const
	{
		auto xElements = std::views::common(points) |
			std::views::transform([](auto pos) { return pos.x; });

		auto it = std::ranges::min_element(xElements);

		if (it != xElements.end())
			return *it;

		return GridPosition::NullCoord;
	}

	u32 SubTexturePickerModal::NewTexture::MaxX() const
	{
		auto xElements = std::views::common(points) |
			std::views::transform([](auto pos) { return pos.x; });

		auto it = std::ranges::max_element(xElements);

		if (it != xElements.end())
			return *it;

		return GridPosition::NullCoord;
	}

	u32 SubTexturePickerModal::NewTexture::MinY() const
	{
		auto yElements = std::views::common(points) |
			std::views::transform([](auto pos) { return pos.y; });

		auto it = std::ranges::min_element(yElements);

		if (it != yElements.end())
			return *it;

		return 0;
	}

	u32 SubTexturePickerModal::NewTexture::MaxY() const
	{
		auto yElements = std::views::common(points) |
			std::views::transform([](auto pos) { return pos.y; });

		auto it = std::ranges::max_element(yElements);

		if (it != yElements.end())
			return *it;

		return 0;
	}

	bool SubTexturePickerModal::NewTexture::contains(const GridPosition& pos)
	{
		return std::ranges::find(points, pos) != points.end();
	}
}