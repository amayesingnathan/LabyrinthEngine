#include "Lpch.h"
#include "ImGuiWidgets.h"

#include <Labyrinth/Editor/EditorResources.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	void LabWidgets::Image(Ref<IRenderable> image, const glm::vec2& size, float rotation)
	{
		if (!image)
			image = EditorResources::NoTexture;

		const glm::vec2* coords = image->getTextureCoords();
		Widgets::Image(image->getTextureID(), size, rotation, coords[3], coords[1]);
	}

	void LabWidgets::ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<>&& action)
	{
		if (!image)
			image = EditorResources::NoTexture;

		const glm::vec2* coords = image->getTextureCoords();
		Widgets::ImageButton(image->getTextureID(), size, std::move(action), coords[3], coords[1]);
	}

	void LabWidgets::TextureSheet(std::string_view id, Ref<Texture2DSheet> sheet, const glm::vec2& size, Action<AssetHandle>&& subtexSelected)
	{
		glm::vec2 cursorPos = Utils::CursorPos<glm::vec2>();

		LabWidgets::Image(sheet, size);

		if (!sheet)
			return;

		glm::vec2 tempCursorPos = Utils::CursorPos<glm::vec2>();
		Utils::SetButtonTransparent();
		Widgets::GridControl<glm::vec2>(cursorPos, size, sheet->getTileCountX(), sheet->getTileCountY(), [=](u32 x, u32 y, const glm::vec2& tileSize)
		{
			Widgets::Button(tileSize, std::format("##{}({}, {})", id, x, y), [=]() { subtexSelected(sheet->getFromPosition({ x, y })); });
		});
		Utils::ResetButtonTransparency();
		Utils::SetCursorPos(tempCursorPos.x, tempCursorPos.y);
	}

	void LabWidgets::TextureSheet(std::string_view id, Ref<Texture2DSheet> sheet, const glm::vec2& size, Action<const GridPosition&>&& subtexSelected)
	{
		glm::vec2 cursorPos = Utils::CursorPos<glm::vec2>();

		LabWidgets::Image(sheet, size);

		if (!sheet)
			return;

		glm::vec2 tempCursorPos = Utils::CursorPos<glm::vec2>();
		Utils::SetButtonTransparent();
		Widgets::GridControl<glm::vec2>(cursorPos, size, sheet->getTileCountX(), sheet->getTileCountY(), [=](u32 x, u32 y, const glm::vec2& tileSize)
		{
			Widgets::Button(tileSize, std::format("##{}({}, {})", id, x, y), [=]() { subtexSelected(GridPosition{ x, y }); });
		});
		Utils::ResetButtonTransparency();
		Utils::SetCursorPos(tempCursorPos.x, tempCursorPos.y);
	}

	bool LabWidgets::ComponentImpl(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<>&& whileOpen)
	{
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;
		bool open = Widgets::TreeNodeEx(id, text, flags);
		bool removeComponent = false;

		f32 lineHeight = Utils::LineHeight();
		Widgets::SameLine(Utils::WindowWidth() - lineHeight);
		Widgets::Button(glm::vec2{ lineHeight }, "+", []() { Widgets::OpenPopup("ComponentSettings"); });

		Widgets::BeginPopup("ComponentSettings");
		Widgets::AddPopupItem("Remove component", [&]() { removeComponent = true; });
		Widgets::EndPopup();

		if (open)
		{
			whileOpen();
			ImGui::TreePop();
		}

		return removeComponent;
	}
}

