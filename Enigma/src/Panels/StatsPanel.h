#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	class Entity;

	class StatsPanel : public Panel
	{
	public:
		void bindHoveredEntity(Entity& entity);

		void onImGuiRender() override;

	private:
		Entity* mHoveredEntity = nullptr;
	};

}