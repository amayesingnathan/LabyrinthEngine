#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	class Entity;

	class StatsPanel : public Panel
	{
	public:
		StatsPanel(Entity& entity) 
			: mHoveredEntity(&entity) {}

		void onImGuiRender() override;

	private:
		Entity* mHoveredEntity = nullptr;
	};

}