#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>

namespace Laby {

	class Entity;

	class StatisticsPanel : public IEditorPanel
	{
	public:
		StatisticsPanel(Entity& entity)
			: mHoveredEntity(entity) {}

		void OnRender() override;

	private:
		Entity& mHoveredEntity;
	};

}