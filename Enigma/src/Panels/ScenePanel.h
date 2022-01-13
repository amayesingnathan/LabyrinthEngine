#pragma once

#include "Labyrinth/Core/Base.h"
#include "Labyrinth/Core/Log.h"
#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"

#include <imgui/imgui_internal.h>

namespace Labyrinth {

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

	private:
		Ref<Scene> mContext;
		Entity mSelectedEntity;
	};

}