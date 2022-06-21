#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	struct EditorData;

	class OptionsPanel : public Panel
	{
	public:
		void setEditorData(EditorData& editorData);

		void onImGuiRender() override;

	private:
		EditorData* mEditorData = nullptr;
	};

}