#pragma once

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/IO/JSON.h>

namespace Laby {

	class SettingsModal : public IEditorModal
	{
	public:
		SettingsModal();

		void onImGuiRender() override;
		void onComplete() override;

	private:
		ApplicationSpec mSettings;
	};

}
