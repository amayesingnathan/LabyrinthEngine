#pragma once

#include <Labyrinth/Editor/EditorData.h>
#include <Labyrinth/Editor/IEditorPanel.h>

namespace Laby {

	class OptionsPanel : public IEditorPanel
	{
	public:
		OptionsPanel(EditorData& data) : mEditorData(&data) {}

		void OnRender() override;

	private:
		EditorData* mEditorData = nullptr;
	};

}