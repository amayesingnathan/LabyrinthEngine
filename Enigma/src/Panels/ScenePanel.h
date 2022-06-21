#pragma once

#include "PanelManager.h"

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"

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

		void BodySpecModalRender();

	private:
		Ref<Scene> mContext = nullptr;
		EditorData* mEditorData = nullptr;

		Entity mSelectedEntity;
		std::vector<Entity> mToRemove;

		BodySpecModal* mBodyCreation = nullptr;
	};

}