#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"

namespace Labyrinth {

	class BodySpecModal;

	class ScenePanel
	{
	public:
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);
		
		void onImGuiRender();

		Entity getSelectedEntity() const { return mSelectedEntity; }
		void setSelectedEntity(Entity entity);

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents();

		void BodySpecModalRender();

	private:
		Ref<Scene> mContext;
		Entity mSelectedEntity;

		std::vector<Entity> mToRemove;

		BodySpecModal* mBodyCreation = nullptr;
	};

}