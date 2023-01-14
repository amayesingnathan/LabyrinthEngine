#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/IRenderable.h>

#include "ImGuiUtils.h"

#include "Widgets/Combobox.h"
#include "Widgets/MenuBar.h"
#include "Widgets/PopUp.h"

struct ImVec2;

namespace Laby {

	class Widgets
	{
	public:
		static void NewLine();
		static void SameLine(f32 xOffset = 0.0f);
		static void Separator();

		static void SetXPosition(f32 pos);
		static void SetYPosition(f32 pos);

		static ImVec2 GetAvailableRegion();

		using GridFunction = std::function<void(const GridPosition&, const glm::vec2&)>;
		static void GridControl(const glm::vec2& pos, const glm::vec2& size, usize width, usize height, GridFunction func);
		static void GridControl(const glm::vec2& size, usize width, usize height, GridFunction func);

		static void TreeNode(const UUID& id, std::string_view text, bool selected, Action<> whileOpen);

		static UI::MenuBar* BeginMenuBar();
		static void AddMenuHeading(UI::MenuBar* context, std::string_view heading);
		static void AddMenuItem(UI::MenuBar* context, std::string_view heading, Action<> action);
		static void AddMenuItem(UI::MenuBar* context, std::string_view heading, std::string_view shortcut, Action<> action);
		static void AddMenuItem(UI::MenuBar* context, std::string_view heading, bool& displayed);
		static void AddMenuSeparator(UI::MenuBar* context);
		static void EndMenuBar(UI::MenuBar* bar);

		static void OpenPopup(std::string_view popupName);
		static UI::PopUp* BeginPopup(std::string_view popupName);
		static void AddMenuItem(UI::PopUp* context, std::string_view heading, Action<> action);
		static void EndPopup(UI::PopUp* popup);

		static UI::PopUpContext* BeginContextPopup();
		static void AddMenuItem(UI::PopUpContext* context, std::string_view heading, Action<> action);
		static void EndContextPopup(UI::PopUpContext* popup);

		static void Label(std::string_view text, ...);

		static void StringEdit(std::string_view label, std::string& field);
		static void PathEdit(std::string_view label, fs::path& field);

		static void IntEdit(std::string_view label, i8& field) { field = (i8)ScalarEdit(label, field); }
		static void IntEdit(std::string_view label, i16& field) { field = (i16)ScalarEdit(label, field); }
		static void IntEdit(std::string_view label, i32& field) { field = (i32)ScalarEdit(label, field); }
		static void IntEdit(std::string_view label, i64& field) { field = (i64)ScalarEdit(label, field); }
		static void UIntEdit(std::string_view label, u8& field) { field = (u8)UScalarEdit(label, field); }
		static void UIntEdit(std::string_view label, u16& field) { field = (u16)UScalarEdit(label, field); }
		static void UIntEdit(std::string_view label, u32& field) { field = (u32)UScalarEdit(label, field); }
		static void UIntEdit(std::string_view label, u64& field) { field = (u64)UScalarEdit(label, field); }

		static void Vector3Edit(std::string_view label, glm::vec3& values, f32 resetVal, f32 colWidth);

		static void Image(Ref<IRenderable> image, const glm::vec2& size);
		static void ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<> action = {}, int padding = -1);

		template<typename T>
		static void AddDragDropSource(std::string_view strID, const T& data) { DragDropSourceInternal(strID, &data, sizeof(T)); }
		template<typename T>
		static void AddDragDropTarget(std::string_view strID, Action<const T&> response)
		{
			void* payloadData = DragDropTargetInternal(strID);
			if (payloadData)
				response(*(T*)payloadData);
		}

		static void OnWidgetSelected(Action<> action);

		static void Checkbox(std::string_view label, bool& value, Action<> action = {});
		static void Button(std::string_view label, Action<> action);

		template<typename T>
		static void Combobox(std::string_view label, std::string_view preview, T& value, const ComboEntry<T> table[])
		{
			usize tableCount = sizeof(table) / sizeof(T);
			const IComboEntry* newTable[] = { nullptr };
			for (usize i = 0; i < tableCount; i++)
				newTable[i] = &table[i];

			const void* comboValue = ComboboxInternal(label, preview, newTable, tableCount);
			LAB_CORE_ASSERT(comboValue, "Combobox had no value!");
			memcpy(&value, comboValue, sizeof(T));
		}

	private:
		static i64 ScalarEdit(std::string_view label, i64 field);
		static u64 UScalarEdit(std::string_view label, u64 field);

		static void DragDropSourceInternal(std::string_view strID, const void* data, usize size);
		static void* DragDropTargetInternal(std::string_view strID);

		static const void* ComboboxInternal(std::string_view label, std::string_view preview, const IComboEntry** table, usize tableCount);
	};
}