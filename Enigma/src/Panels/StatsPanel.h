#pragma once

#include <Labyrinth/Editor/EditorPanel.h>

namespace Labyrinth {

	class Entity;

	class StatsPanel : public EditorPanel
	{
	public:
		StatsPanel(Entity& entity) 
			: mHoveredEntity(entity) {}

		void onImGuiRender() override;

		static Ref<StatsPanel> Create(Entity& entity) { return Ref<StatsPanel>::Create(entity); }

	private:
		Entity& mHoveredEntity;
	};

}