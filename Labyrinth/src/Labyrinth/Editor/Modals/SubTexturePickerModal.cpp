#include "Lpch.h"
#include "SubTexturePickerModal.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	SubTexturePickerModal::SubTexturePickerModal(Ref<Texture2DSheet> context)
		: mCurrentSheet(context)
	{
	}

	void SubTexturePickerModal::onImGuiRender()
	{
		const glm::vec2& region = Utils::AvailableRegion<glm::vec2>();
		glm::vec2 sheetRegion = { 0.75f * region.x, region.y };
		Widgets::BeginChild("SpriteSheetDisplay", sheetRegion);

		LabWidgets::TextureSheet("SheetPicker", mCurrentSheet, sheetRegion, [this](const GridPosition& pos)
		{
			if (mCurrentlyAdding.contains(pos))
				std::erase(mCurrentlyAdding.points, pos);
			else
				mCurrentlyAdding.points.push_back(pos);
		});

		Widgets::EndChild();

		Widgets::SameLine();

		Widgets::BeginChild("SubTexturesAdded");


		Widgets::EndChild();
	}

	void SubTexturePickerModal::onCustomButtonRender(bool& open)
	{
	}

	void SubTexturePickerModal::onComplete()
	{
	}

	bool SubTexturePickerModal::NewTexture::valid() const
	{
		GridPosition bottomLeft = { MinX(), MinY() };
		GridPosition topRight = { MaxX(), MaxY() };

		if (!bottomLeft.valid() || !topRight.valid())
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
		u32 expectedCount = (topRight.x - bottomLeft.x) * (topRight.y - bottomLeft.y);
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