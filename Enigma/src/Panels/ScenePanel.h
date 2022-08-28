#pragma once

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Editor/EditorPanel.h>
#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Labyrinth {

	struct EditorData;

	class ScenePanel : public EditorPanel
	{
	public:
		ScenePanel();
		ScenePanel(EditorData& options);
		ScenePanel(const Ref<Scene>& scene, EditorData& options);

		void setContext(const Ref<Scene>& scene);
		void setContext(const Ref<Scene>& scene, EditorData& options);
		
		void onUpdate(Timestep ts) override;
		void onImGuiRender() override;
		void onSelectionChange() override;

		static Ref<ScenePanel> Create(EditorData& options) { return Ref<ScenePanel>::Create(options); }
		static Ref<ScenePanel> Create(const Ref<Scene>& scene, EditorData& options) { return Ref<ScenePanel>::Create(scene, options); }

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents();

		template<typename T>
		void DrawAddComponentEntry(const std::string& text)
		{
			if (!mSelectedEntity.hasComponent<T>())
			{
				if (ImGui::MenuItem(text.c_str()))
				{
					mSelectedEntity.addComponent<T>();
					ImGui::CloseCurrentPopup();
				}
			}
		}

		void DrawChildControllerElement(const std::string& name, glm::vec3& componentElement, glm::vec3& displayElement, glm::vec3& lastDisplay, float min, float max, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

		void AssetTypeWarning();

	private:
		Ref<Scene> mContext = nullptr;
		EditorData* mEditorData = nullptr;

		Entity mSelectedEntity;
		Entity mPreviousEntity;
		std::vector<Entity> mToRemove;

		Ref<Framebuffer> mTexture = nullptr;

		struct TexTypes { std::string label;  SpriteRendererComponent::TexType type; };
		const std::vector<TexTypes> mTexTypes =
		{
			{ "Colour", SpriteRendererComponent::TexType::None },
			{ "Texture2D", SpriteRendererComponent::TexType::Texture },
			{ "SubTexture2D", SpriteRendererComponent::TexType::SubTexture }
		};
	};

}