#pragma once

#include "PanelManager.h"

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	class BodySpecModal;
	struct EditorData;

	class ScenePanel : public Panel
	{
	public:
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& scene, EditorData* options);

		void setContext(const Ref<Scene>& scene);
		void setContext(const Ref<Scene>& scene, EditorData* options);
		
		void onImGuiRender();

		Entity getSelectedEntity() const { return mSelectedEntity; }
		void setSelectedEntity(Entity entity);

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents();

		void AssetTypeWarning();
		void BodySpecModalRender();

	private:
		Ref<Scene> mContext = nullptr;
		EditorData* mEditorData = nullptr;

		Entity mSelectedEntity;
		std::vector<Entity> mToRemove;

		BodySpecModal* mBodyCreation = nullptr;

		struct TexTypes { std::string label;  SpriteRendererComponent::TexType type; };
		const std::vector<TexTypes> mTexTypes =
		{
			{ "Colour", SpriteRendererComponent::TexType::None },
			{ "Texture2D", SpriteRendererComponent::TexType::Texture },
			{ "Circle", SpriteRendererComponent::TexType::Tile }
		};
	};

}