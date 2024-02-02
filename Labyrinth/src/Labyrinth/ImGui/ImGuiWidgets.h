#pragma once

#include "ImGui/Widgets.h"

#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	using Widgets = slc::Widgets;
	using Utils = slc::Utils;

	class LabWidgets
	{
	public:
		template<typename T>
		static void Component(std::string_view name, Entity entity, Action<T&>&& uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = 0b110000100110;
			// ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (!entity.HasComponent<T>())
				return;

			auto& component = entity.GetComponent<T>();
			const glm::vec2& contentRegionAvailable = Utils::AvailableRegion<glm::vec2>();

			Utils::PushStyle(11, Utils::ToImVec<ImVec2>(glm::vec2{ 4, 4 })); //ImGuiStyleVar_FramePadding
			f32 lineHeight = Utils::FontSize() + Utils::FramePadding().y * 2.0f;
			Widgets::Separator();

			bool removeComponent = ComponentImpl((void*)typeid(T).hash_code(), name, false, treeNodeFlags, [&]() { uiFunction(component); });
			if (removeComponent)
				entity.RemoveComponent<T>();

			Utils::PopStyle();
		}

		static void Image(Ref<IRenderable> image, const glm::vec2& size, float rotation = 0.0f);
		static void ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<>&& action = {});

		static void TextureSheet(std::string_view id, Ref<Texture2DSheet> sheet, const glm::vec2& size, Action<AssetHandle>&& subtexSelected = [](AssetHandle) {});
		static void TextureSheet(std::string_view id, Ref<Texture2DSheet> sheet, const glm::vec2& size, Action<const GridPosition&>&& subtexSelected = [](const GridPosition&) {});

	private:
		static bool ComponentImpl(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<>&& whileOpen);
	};
}