#pragma once

#include <Labyrinth/Editor/EditorPanel.h>

namespace Labyrinth {

	struct EditorData;

	class OptionsPanel : public EditorPanel
	{
	public:
		OptionsPanel(EditorData& data) : mEditorData(&data) {}

		void onImGuiRender() override;

		static Ref<OptionsPanel> Create(EditorData& data) { return Ref<OptionsPanel>::Create(data); }

	private:
		EditorData* mEditorData = nullptr;
	};

}