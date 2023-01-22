#pragma once

#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/IRenderable.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Tools/StringUtils.h>

#include "ImGuiUtils.h"

#include "Widgets/Combobox.h"
#include "Widgets/MenuBar.h"
#include "Widgets/Payload.h"
#include "Widgets/PopUp.h"

namespace Laby {

	class Widgets
	{
	public:
		static void NewLine();
		static void SameLine(f32 xOffset = 0.0f);
		static void Separator();

		static void SetXPosition(f32 pos);
		static void SetYPosition(f32 pos);

		using GridFunction = std::function<void(const GridPosition&, const glm::vec2&)>;
		static void GridControl(const glm::vec2& pos, const glm::vec2& size, usize width, usize height, GridFunction func);
		static void GridControl(const glm::vec2& size, usize width, usize height, GridFunction func);

		static void BeginColumns(i32 count, bool border = false);
		static void NextColumn();
		static void EndColumns();

		static void TreeNode(void* id, std::string_view text, bool selected, Action<> whileOpen);

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

		static void Label(std::string_view fmt);
		static void LabelWrapped(std::string_view fmt);

		static void StringEdit(std::string_view label, std::string& field);
		static void PathEdit(std::string_view label, fs::path& field);

		template<SignedIntegral T>
		static void IntEdit(std::string_view label, T& field)
		{
			i64 result = ScalarEdit(label, field);
			if (result < Limits<T>::Min)
				result = Limits<T>::Min;
			else if (result > Limits<T>::Max)
				result = Limits<T>::Max;

			field = (T)result;
		}
		template<SignedIntegral T>
		static void IntEdit(std::string_view label, T field, Action<T> onEdit)
		{
			i64 result = ScalarEdit(label, field);
			if (result < Limits<T>::Min)
				result = Limits<T>::Min;
			else if (result > Limits<T>::Max)
				result = Limits<T>::Max;

			field = (T)result;
			onEdit(field);
		}

		template<UnsignedIntegral T>
		static void UIntEdit(std::string_view label, T& field)
		{
			u64 result = UScalarEdit(label, field);
			if (result < Limits<T>::Min)
				result = Limits<T>::Min;
			else if (result > Limits<T>::Max)
				result = Limits<T>::Max;

			field = (T)result;
		}
		template<UnsignedIntegral T>
		static void UIntEdit(std::string_view label, T field, Action<T> onEdit)
		{
			u64 result = UScalarEdit(label, field);
			if (result < Limits<T>::Min)
				result = Limits<T>::Min;
			else if (result > Limits<T>::Max)
				result = Limits<T>::Max;

			field = (T)result;
			onEdit(field);
		}

		static void FloatEdit(std::string_view label, f32& field, f32 speed = 1.0f, f32 mix = 0.0f, f32 max = 0.0f);
		static void DoubleEdit(std::string_view label, f64& field, f32 speed = 1.0f, f32 mix = 0.0f, f32 max = 0.0f);
		static void FloatEdit(std::string_view label, f32 field, Action<f32> onEdit, f32 speed = 1.0f, f32 mix = 0.0f, f32 max = 0.0f);
		static void DoubleEdit(std::string_view label, f64 field, Action<f64> onEdit, f32 speed = 1.0f, f32 mix = 0.0f, f32 max = 0.0f);

		static void Vector2Edit(std::string_view label, glm::vec2& values, f32 resetVal = 0.0f, f32 colWidth = 100.0f);
		static void Vector3Edit(std::string_view label, glm::vec3& values, f32 resetVal = 0.0f, f32 colWidth = 100.0f);
		static void Vector4Edit(std::string_view label, glm::vec4& values, f32 resetVal = 0.0f, f32 colWidth = 100.0f);
		static void Vector2Edit(std::string_view label, glm::vec2 values, Action<const glm::vec2&> onEdit, f32 resetVal = 0.0f, f32 colWidth = 100.0f);
		static void Vector3Edit(std::string_view label, glm::vec3 values, Action<const glm::vec3&> onEdit, f32 resetVal = 0.0f, f32 colWidth = 100.0f);
		static void Vector4Edit(std::string_view label, glm::vec4 values, Action<const glm::vec4&> onEdit, f32 resetVal = 0.0f, f32 colWidth = 100.0f);

		static void ColourEdit(std::string_view label, glm::vec4& colour);

		static void Image(Ref<IRenderable> image, const glm::vec2& size);
		static void ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<> action = {}, int padding = -1);

		template<typename T>
		static void AddDragDropSource(std::string_view strID, const T& data) 
		{ 
			DragDropSourceInternal(strID, [&](){ sCurrentPayload = MakeSingle<Payload<T>>(data); });
		}
		template<typename T>
		static void AddDragDropTarget(std::string_view strID, Action<const T&> response)
		{
			void* imguiPayload = DragDropTargetInternal(strID);
			if (!imguiPayload)
				return;

			const T& payload = *(T*)imguiPayload;
			response(payload);
		}

		static void OnWidgetSelected(Action<> action);

		static void Checkbox(std::string_view label, bool& value, Action<> action = {});
		static void Button(std::string_view label, Action<> action = {});
		static void Button(std::string_view label, const glm::vec2& size, Action<> action = {});

		template<typename T>
		static void Combobox(std::string_view label, std::string_view preview, T& value, const ComboEntry<T>* table, usize tableLength)
		{
			std::vector<const IComboEntry*> internalTable(tableLength);
			for (usize i = 0; i < tableLength; i++)
				internalTable[i] = &table[i];

			const IComboEntry* comboEntry = ComboboxInternal(label, preview, internalTable);
			if (!comboEntry)
				return;

			const void* comboValue = comboEntry->getVal();
			LAB_CORE_ASSERT(comboValue, "Combobox had no value!");
			memcpy(&value, comboValue, sizeof(T));
		}

		template<typename T, typename Func>
		static void Combobox(std::string_view label, std::string_view preview, T value, const ComboEntry<T>* table, usize tableLength, Func onEdit)
		{
			std::vector<const IComboEntry*> internalTable(tableLength);
			for (usize i = 0; i < tableLength; i++)
				internalTable[i] = &table[i];

			const IComboEntry* comboEntry = ComboboxInternal(label, preview, internalTable);
			if (!comboEntry)
				return;

			const void* comboValue = comboEntry->getVal();
			LAB_CORE_ASSERT(comboValue, "Combobox had no value!");
			onEdit(comboEntry->key, *(const T*)comboValue);
		}

		template<typename T>
		static void Component(std::string_view name, Entity entity, Action<T&> uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = 0b110000100110; 
			// ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity.hasComponent<T>())
			{
				auto& component = entity.getComponent<T>();
				const glm::vec2& contentRegionAvailable = ImGuiUtils::AvailableRegion();

				ImGuiUtils::PushStyle(11, glm::vec2{ 4, 4 }); //ImGuiStyleVar_FramePadding
				f32 lineHeight = ImGuiUtils::FontSize() + ImGuiUtils::FramePadding().y * 2.0f;
				Separator();

				bool removeComponent = ComponentInternal((void*)typeid(T).hash_code(), name, false, treeNodeFlags, [&]() { uiFunction(component); });
				if (removeComponent)
					entity.removeComponent<T>();

				ImGuiUtils::PopStyle();
			}
		}

	private:
		static i64 ScalarEdit(std::string_view label, i64 field);
		static void ScalarEdit(std::string_view label, i64 field, Action<i64> onEdit);
		static u64 UScalarEdit(std::string_view label, u64 field);
		static void UScalarEdit(std::string_view label, u64 field, Action<u64> onEdit);

		static bool ComponentInternal(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<> whileOpen);
		static void TreeNodeInternal(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<> whileOpen);

		static void DragDropSourceInternal(std::string_view strID, Action<> createPayload);
		static void* DragDropTargetInternal(std::string_view strID);

		static const IComboEntry* ComboboxInternal(std::string_view label, std::string_view preview, const std::vector<const IComboEntry*>& table);

	private:
		inline static Single<IPayload> sCurrentPayload = nullptr;
	};
}