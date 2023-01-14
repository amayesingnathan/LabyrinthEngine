#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

namespace Laby {

	class ScenePanel : public IEditorPanel
	{
	public:
		ScenePanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene) { mContext = scene; }

		void onImGuiRender() override;
		void onSelectionChange() override;

	private:
		void DrawEntityNode(Entity entity);

	private:
		Ref<Scene> mContext = nullptr;

		Entity mSelectedEntity;
		Entity mPreviousEntity;
		std::vector<Entity> mToRemove;
	};

}