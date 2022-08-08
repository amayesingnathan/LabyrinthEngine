#pragma once

#include "PanelManager.h"

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Renderer/Framebuffer.h"
#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Labyrinth {

	struct EditorData;

	class ScenePanel : public Panel
	{
	public:
		ScenePanel();
		ScenePanel(EditorData& options);
		ScenePanel(const Ref<Scene>& scene, EditorData& options);

		void setContext(const Ref<Scene>& scene);
		void setContext(const Ref<Scene>& scene, EditorData& options);
		
		void onUpdate() override;
		void onImGuiRender() override;

		Entity getSelectedEntity() const { return mSelectedEntity; }
		void setSelectedEntity(Entity entity);

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

		void AssetTypeWarning();

	private:
		Ref<Scene> mContext = nullptr;
		EditorData* mEditorData = nullptr;

		Entity mSelectedEntity;
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