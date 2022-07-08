#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	class Entity;

	class StatsPanel : public IPanel
	{
	public:
		StatsPanel(Entity& entity) 
			: mHoveredEntity(&entity) {}

		void onImGuiRender() override;

		static Ref<StatsPanel> Create(Entity& entity) { return CreateRef<StatsPanel>(entity); }

	private:
		Entity* mHoveredEntity = nullptr;
	};

}