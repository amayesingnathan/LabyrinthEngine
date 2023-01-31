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

	void Widgets::Disable(bool disable)
	{
		ImGui::BeginDisabled(disable);
	}

	void Widgets::EndDisable()
	{
		ImGui::EndDisabled();
	}

	void Widgets::SetXPosition(f32 pos)
	{
		ImGui::SetCursorPosX(pos);
	}

	void Widgets::SetYPosition(f32 pos)
	{
		ImGui::SetCursorPosY(pos);
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

		for (u32 y = 0; y < width; y++)
		{
			for (u32 x = 0; x < height; x++)
			{
				ImGui::SetCursorPosX(pos.x + ((f32)x * tileSize.x));
				ImGui::SetCursorPosY(pos.y + ((f32)y * tileSize.y));

				func({ x, y }, tileSize);
			}
		}
	}

	void Widgets::BeginColumns(i32 count, bool border)
	{
		ImGui::Columns(count, 0, border);
	}

	void Widgets::NextColumn()
	{
		ImGui::NextColumn();
	}

	void Widgets::EndColumns()
	{
		ImGui::Columns(1);
	}

	void Widgets::BeginChild(std::string_view strID, const glm::vec2& size, bool border)
	{
		ImGui::BeginChild(strID.data(), ImGuiUtils::FromGLM(size));
	}

	void Widgets::EndChild()
	{
		ImGui::EndChild();
	}

	void Widgets::BeginGroup()
	{
		ImGui::BeginGroup();
	}

	void Widgets::EndGroup()
	{
		ImGui::EndGroup();
	}

	void Widgets::TreeNode(void* id, std::string_view text, bool selected, Action<> whileOpen)
	{
		TreeNodeInternal(id, text, selected, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth, whileOpen);
	}

	void Widgets::Selectable(std::string_view label, bool selected, Action<> action)
	{
		if (ImGui::Selectable(label.data(), selected))
			action();
	}

	bool Widgets::ComponentInternal(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<> whileOpen)
	{
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;
		bool open = ImGui::TreeNodeEx((void*)&id, flags, text.data());
		bool removeComponent = false;

		SameLine(ImGuiUtils::WindowWidth() - 0.5f * ImGuiUtils::LineHeight());
		Button("+", []() { OpenPopup("ComponentSettings"); });

		UI::PopUp* popup = BeginPopup("ComponentSettings");
		AddMenuItem(popup, "Remove component", [&]() { removeComponent = true;});
		EndPopup(popup);
		
		if (open)
		{
			whileOpen();
			ImGui::TreePop();
		}

		return removeComponent;
	}

	void Widgets::TreeNodeInternal(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<> whileOpen)
	{
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
		context->addMenuItemAction(heading, "", action);
	}

	void Widgets::AddMenuItem(UI::MenuBar* context, std::string_view heading, std::string_view shortcut, Action<> action)
	{
		context->addMenuItemAction(heading, shortcut, action);
	}

	void Widgets::AddMenuItem(UI::MenuBar* context, std::string_view heading, bool& displayed)
	{
		context->addMenuItemSwitch(heading, "", displayed);
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

	void Widgets::Label(std::string_view text)
	{
		if (text.empty())
		{
			ImGui::Text("...");
			return;
		}

		ImGui::Text(text.data());
	}

	void Widgets::LabelWrapped(std::string_view text)
	{
		if (text.empty())
		{
			ImGui::TextWrapped("...");
			return;
		}
		ImGui::TextWrapped(text.data());
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

	void Widgets::DragDropSourceInternal(std::string_view strID, Action<> createPayload)
	{
		if (!ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			return;

		createPayload();
		ImGui::SetDragDropPayload(strID.data(), sCurrentPayload->data(), sCurrentPayload->size());
		ImGui::EndDragDropSource();
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

	void Widgets::OnWidgetHovered(Action<> action)
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
			action();
	}

	void Widgets::Checkbox(std::string_view label, bool& value, Action<> action)
	{
		if (ImGui::Checkbox(label.data(), &value) && action)
			action();
	}

	void Widgets::Button(std::string_view label, Action<> action)
	{
		if (ImGui::Button(label.data()) && action)
			action();
	}

	void Widgets::Button(std::string_view label, const glm::vec2& size, Action<> action)
	{
		if (ImGui::Button(label.data(), ImGuiUtils::FromGLM(size)) && action)
			action();
	}

	void Widgets::FloatEdit(std::string_view label, f32& field, f32 speed, f32 mix, f32 max)
	{
		ImGui::DragFloat(label.data(), &field);
	}

	void Widgets::FloatEdit(std::string_view label, f32 field, Action<f32> onEdit, f32 speed, f32 mix, f32 max)
	{
		if (ImGui::DragFloat(label.data(), &field))
			onEdit(field);
	}

	void Widgets::DoubleEdit(std::string_view label, f64 field, Action<f64> onEdit, f32 speed, f32 mix, f32 max)
	{
		f32 tmp = (f32)field;
		if (ImGui::DragFloat(label.data(), &tmp))
		{
			field = (f64)tmp;
			onEdit(field);
		}
	}

	void Widgets::DoubleEdit(std::string_view label, f64& field, f32 speed, f32 mix, f32 max)
	{
		f32 tmp = (f32)field;
		ImGui::DragFloat(label.data(), &tmp);
		field = (f64)tmp;
	}

	void Widgets::Vector2Edit(std::string_view label, glm::vec2& values, f32 resetVal, f32 colWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.data());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
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

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
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

	void Widgets::Vector4Edit(std::string_view label, glm::vec4& values, f32 resetVal, f32 colWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.data());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize))
			values.w = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void Widgets::Vector2Edit(std::string_view label, glm::vec2 values, Action<const glm::vec2&> onEdit, f32 resetVal, f32 colWidth)
	{
		glm::vec2 tmp = values;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.data());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
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

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		if (tmp != values)
			onEdit(values);
	}

	void Widgets::Vector3Edit(std::string_view label, glm::vec3 values, Action<const glm::vec3&> onEdit, f32 resetVal, f32 colWidth)
	{
		glm::vec3 tmp = values;

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

		if (tmp != values)
			onEdit(values);
	}

	void Widgets::Vector4Edit(std::string_view label, glm::vec4 values, Action<const glm::vec4&> onEdit, f32 resetVal, f32 colWidth)
	{
		glm::vec4 tmp = values;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.data());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize))
			values.w = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		if (tmp != values)
			onEdit(values);
	}

	void Widgets::ColourEdit(std::string_view label, glm::vec4& colour)
	{
		ImGui::ColorEdit4(label.data(), &colour.x);
	}

	void Widgets::Image(Ref<IRenderable> image, const glm::vec2& size, f32 rotation)
	{
		if (!image)
			return;

		const glm::vec2* coords = image->getTextureCoords();

		const ImVec2& uv0 = ImGuiUtils::FromGLM(coords[3]);
		const ImVec2& uv1 = ImGuiUtils::FromGLM(coords[1]);

		ImGui::Image((ImTextureID)(uintptr_t)image->getTextureID(), ImGuiUtils::FromGLM(size), uv0, uv1);

		//static constexpr glm::vec2 defaultCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		//const ImVec2* coords = (ImVec2*)image->getTextureCoords();
		//if (!coords)
		//	coords = (ImVec2*)defaultCoords;

		//ImDrawList* draw_list = ImGui::GetWindowDrawList();
		//glm::vec2 centre = ImGuiUtils::CursorPos() + 0.5f * size;

		//f32 cos_a = cosf(rotation);
		//f32 sin_a = sinf(rotation);
		//glm::vec2 pos[4] =
		//{
		//	centre + ImGuiUtils::ToGLM(ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a)),
		//	centre + ImGuiUtils::ToGLM(ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a)),
		//	centre + ImGuiUtils::ToGLM(ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)),
		//	centre + ImGuiUtils::ToGLM(ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a))
		//};
		//ImVec2* imPos = (ImVec2*)pos;

		//draw_list->AddImageQuad((ImTextureID)(uintptr_t)image->getTextureID(), 
		//	imPos[0], imPos[1], imPos[2], imPos[3], 
		//	coords[0], coords[1], coords[2], coords[3], 
		//	IM_COL32_WHITE);
	}

	void Widgets::ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<> action, int padding)
	{
		const glm::vec2* coords = image->getTextureCoords();
		const ImVec2& uv0 = ImGuiUtils::FromGLM(coords[3]);
		const ImVec2& uv1 = ImGuiUtils::FromGLM(coords[1]);

		if (ImGui::ImageButton((ImTextureID)(uintptr_t)image->getTextureID(), ImGuiUtils::FromGLM(size), uv0, uv1, padding) && action)
			action();
	}

	i64 Widgets::ScalarEdit(std::string_view label, i64 field)
	{
		i32 width = (i32)field;
		ImGui::InputInt(label.data(), &width);
		return (i64)width;
	}

	void Widgets::ScalarEdit(std::string_view label, i64 field, Action<i64> onEdit)
	{
		i32 val = (i32)field;
		ImGui::InputInt(label.data(), &val);
		onEdit((i64)val);
	}

	u64 Widgets::UScalarEdit(std::string_view label, u64 field)
	{
		i32 val = (i32)field;
		ImGui::InputInt(label.data(), &val);
		if (val < 0)
			val = 0;
		return (u64)val;
	}

	void Widgets::UScalarEdit(std::string_view label, u64 field, Action<u64> onEdit)
	{
		i32 val = (i32)field;
		ImGui::InputInt(label.data(), &val);
		if (val < 0)
			val = 0;
		onEdit((u64)val);
	}

	bool Widgets::BeginCombo(std::string_view label, std::string_view preview)
	{
		return ImGui::BeginCombo(label.data(), preview.data());
	}

	const IComboEntry* Widgets::ComboboxEntry(std::string_view preview, const IComboEntry* entry)
	{
		const IComboEntry* result = nullptr;
		bool isSelected = entry->key == preview;

		if (ImGui::Selectable(entry->key.data(), isSelected))
			result = entry;

		if (isSelected)
			ImGui::SetItemDefaultFocus();

		return result;
	}

	void Widgets::EndCombo()
	{
		ImGui::EndCombo();
	}
}