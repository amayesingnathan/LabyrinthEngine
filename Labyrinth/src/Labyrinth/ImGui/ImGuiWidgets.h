#pragma once

#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Renderer/SubTexture.h>

#include "ImGuiCpp.h"

namespace Laby {

	class LabWidgets
	{
	public:
		template<typename T>
		static void Component(std::string_view name, Entity entity, Action<T&>&& uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = 0b110000100110;
			// ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (!entity.hasComponent<T>())
				return;

			auto& component = entity.getComponent<T>();
			const glm::vec2& contentRegionAvailable = imcpp::Utils::AvailableRegion<glm::vec2>();

			imcpp::Utils::PushStyle(11, imcpp::Utils::ToImVec<ImVec2>(glm::vec2{ 4, 4 })); //ImGuiStyleVar_FramePadding
			f32 lineHeight = imcpp::Utils::FontSize() + imcpp::Utils::FramePadding().y * 2.0f;
			imcpp::Widgets::Separator();

			bool removeComponent = ComponentImpl((void*)typeid(T).hash_code(), name, false, treeNodeFlags, [&]() { uiFunction(component); });
			if (removeComponent)
				entity.removeComponent<T>();

			imcpp::Utils::PopStyle();
		}

		static void Image(Ref<IRenderable> image, const glm::vec2& size, float rotation = 0.0f);
		static void ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<>&& action = {});

		static void TextureSheet(std::string_view id, Ref<Texture2DSheet> sheet, const glm::vec2& size, Action<AssetHandle>&& subtexSelected = [](AssetHandle) {});

	private:
		static bool ComponentImpl(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<>&& whileOpen);
	};
}