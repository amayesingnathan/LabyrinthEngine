#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	struct EditorData;

	class OptionsPanel : public Panel
	{
	public:
		OptionsPanel(EditorData& data) : mEditorData(&data) {}

		void onImGuiRender() override;

	private:
		EditorData* mEditorData = nullptr;
	};

}