#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>

namespace Laby {

	class Entity;

	class StatisticsPanel : public IEditorPanel
	{
	public:
		StatisticsPanel(Entity& entity)
			: mHoveredEntity(entity) {}

		void onImGuiRender() override;

	private:
		Entity& mHoveredEntity;
	};

}