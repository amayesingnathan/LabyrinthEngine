#include "Lpch.h"
#include "ImGuiWidgets.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <Labyrinth/Containers/StaticString.h>

#include "ImGuiUtils.h"

namespace Laby {

	void Widgets::NewLine()
	{
		ImGui::NewLine();
	}

	void Widgets::SameLine(f32 xOffset)
	{
		ImGui::SameLine(xOffset);
	}

	void Widgets::Separator()
	{
		ImGui::Separator();
	}

	void Widgets::SetXPosition(f32 pos)
	{
		ImGui::SetCursorPosX(pos);
	}

	void Widgets::SetYPosition(f32 pos)
	{
		ImGui::SetCursorPosY(pos);
	}

	ImVec2 Widgets::GetAvailableRegion()
	{
		return ImGui::GetWindowContentRegionMax();
	}

	void Widgets::GridControl(const glm::vec2& pos, const glm::vec2& size, usize width, usize height, GridFunction func)
	{
		ImGui::SetCursorPos(ImGuiUtils::FromGLM(pos));
		GridControl(size, width, height, func);
	}

	void Widgets::GridControl(const glm::vec2& size, usize width, usize height, GridFunction func)
	{
		ImVec2 pos = ImGui::GetCursorPos();
		glm::vec2 tileSize = { size.x / width, size.y / height };

		for (usize y = 0; y < width; y++)
		{
			for (usize x = 0; x < height; x++)
			{
				ImGui::SetCursorPosX(pos.x + (x * tileSize.x));
				ImGui::SetCursorPosY(pos.y + (y * tileSize.y));

				func({ x, y }, tileSize);
			}
		}
	}

	void Widgets::TreeNode(const UUID& id, std::string_view text, bool selected, Action<> whileOpen)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;
		if (ImGui::TreeNodeEx((void*)&id, flags, text.data()))
		{
			whileOpen();
			ImGui::TreePop();
		}
	}

	UI::MenuBar* Widgets::BeginMenuBar()
	{
		UI::MenuBar* newMenu = new UI::MenuBar;
		return newMenu;
	}

	void Widgets::AddMenuHeading(UI::MenuBar* context, std::string_view heading)
	{
		context->addHeading(heading);
	}

	void Widgets::AddMenuItem(UI::MenuBar* context, std::string_view heading, Action<> action)
	{
		context->addMenuItemAction(heading, nullptr, action);
	}

	void Widgets::AddMenuItem(UI::MenuBar* context, std::string_view heading, std::string_view shortcut, Action<> action)
	{
		context->addMenuItemAction(heading, shortcut, action);
	}

	void Widgets::AddMenuItem(UI::MenuBar* context, std::string_view heading, bool& displayed)
	{
		context->addMenuItemSwitch(heading, nullptr, displayed);
	}

	void Widgets::AddMenuSeparator(UI::MenuBar* context)
	{
		context->addSeparator();
	}

	void Widgets::EndMenuBar(UI::MenuBar* bar)
	{
		delete bar;
	}

	void Widgets::OpenPopup(std::string_view popupName)
	{
		ImGui::OpenPopup(popupName.data());
	}

	UI::PopUp* Widgets::BeginPopup(std::string_view popupName)
	{
		UI::PopUp* newMenu = new UI::PopUp(popupName);
		return newMenu;
	}

	void Widgets::AddMenuItem(UI::PopUp* context, std::string_view heading, Action<> action)
	{
		context->addPopUpItem(heading, action);
	}

	void Widgets::EndPopup(UI::PopUp* popup)
	{
		delete popup;
	}

	UI::PopUpContext* Widgets::BeginContextPopup()
	{
		UI::PopUpContext* newMenu = new UI::PopUpContext();
		return newMenu;
	}

	void Widgets::AddMenuItem(UI::PopUpContext* context, std::string_view heading, Action<> action)
	{
		context->addPopUpItem(heading, action);
	}

	void Widgets::EndContextPopup(UI::PopUpContext* popup)
	{
		delete popup;
	}

	void Widgets::Label(std::string_view text, ...)
	{
		if (text.empty())
		{
			ImGui::Text("...");
			return;
		}

		va_list args;
		va_start(args, text);
		ImGui::Text(text.data(), args);
		va_end(args);
	}

	void Widgets::StringEdit(std::string_view label, std::string& field)
	{
		StaticString<256> stringEditBuffer(field);
		if (ImGui::InputText(label.data(), stringEditBuffer, stringEditBuffer.length()))
			field = stringEditBuffer.toString();
	}

	void Widgets::PathEdit(std::string_view label, fs::path& field)
	{
		StaticString<256> stringEditBuffer(field.string());
		if (ImGui::InputText(label.data(), stringEditBuffer, stringEditBuffer.length()))
			field = stringEditBuffer.toString();
	}

	void Widgets::DragDropSourceInternal(std::string_view strID, const void* data, usize size)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload(strID.data(), data, size);
			ImGui::EndDragDropSource();
		}
	}

	void* Widgets::DragDropTargetInternal(std::string_view strID)
	{
		if (!ImGui::BeginDragDropTarget())
			return nullptr;

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(strID.data());
		ImGui::EndDragDropTarget();

		if (!payload)
			return nullptr;
		return payload->Data;
	}

	void Widgets::OnWidgetSelected(Action<> action)
	{
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			action();
	}

	void Widgets::Checkbox(std::string_view label, bool& value, Action<> action)
	{
		if (ImGui::Checkbox(label.data(), &value))
			action();
	}

	void Widgets::Button(std::string_view label, Action<> action)
	{
		if (ImGui::Button(label.data()))
			action();
	}

	void Widgets::Vector3Edit(std::string_view label, glm::vec3& values, f32 resetVal, f32 colWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.data());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void Widgets::Image(Ref<IRenderable> image, const glm::vec2& size)
	{
		const glm::vec2* coords = image->getTextureCoords();
		ImVec2 uv0 = coords ? ImGuiUtils::FromGLM(coords[3]) : ImVec2{ 0, 1 };
		ImVec2 uv1 = coords ? ImGuiUtils::FromGLM(coords[1]) : ImVec2{ 1, 0 };

		ImGui::Image((ImTextureID)(uintptr_t)image->getTextureID(), ImGuiUtils::FromGLM(size), uv0, uv1);
	}

	void Widgets::ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<> action, int padding)
	{
		const glm::vec2* coords = image->getTextureCoords();
		ImVec2 uv0 = coords ? ImGuiUtils::FromGLM(coords[3]) : ImVec2{ 0, 1 };
		ImVec2 uv1 = coords ? ImGuiUtils::FromGLM(coords[1]) : ImVec2{ 1, 0 };

		if (ImGui::ImageButton((ImTextureID)(uintptr_t)image->getTextureID(), ImGuiUtils::FromGLM(size), uv0, uv1, padding))
			action();
	}

	i64 Widgets::ScalarEdit(std::string_view label, i64 field)
	{
		i32 width = (i32)field;
		ImGui::InputInt(label.data(), &width);
		return (i64)width;
	}

	u64 Widgets::UScalarEdit(std::string_view label, u64 field)
	{
		i32 val = (i32)field;
		ImGui::InputInt(label.data(), &val);
		if (val < 0)
			val = 0;
		return (u64)val;
	}

	const void* Widgets::ComboboxInternal(std::string_view label, std::string_view preview, const IComboEntry** table, usize tableCount)
	{
		const void* result = nullptr;
		if (ImGui::BeginCombo(label.data(), preview.data()))
		{
			for (usize i = 0; i < tableCount; i++)
			{
				const IComboEntry* entry = table[i];
				bool isSelected = entry->key == preview;

				if (ImGui::Selectable(entry->key.data(), isSelected))
					result = entry->getVal();

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		return result;
	}
}