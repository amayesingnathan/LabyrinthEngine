#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	struct EditorData;

	class OptionsPanel : public IPanel
	{
	public:
		OptionsPanel(EditorData& data) : mEditorData(&data) {}

		void onImGuiRender() override;

		static Ref<OptionsPanel> Create(EditorData& data) { return CreateRef<OptionsPanel>(data); }

	private:
		EditorData* mEditorData = nullptr;
	};

}