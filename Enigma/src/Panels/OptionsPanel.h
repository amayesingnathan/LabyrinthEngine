#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	struct EditorData;

	class OptionsPanel : public Panel
	{
	public:
		OptionsPanel(EditorData& data) : mEditorData(&data) {}

		void onImGuiRender() override;

		static Ref<OptionsPanel> Create(EditorData& data) { return Ref<OptionsPanel>::Create(data); }

	private:
		EditorData* mEditorData = nullptr;
	};

}