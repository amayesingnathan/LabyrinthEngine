#pragma once

#include <Labyrinth/Editor/EditorModal.h>

#include <Labyrinth/IO/JSON.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SettingsModal : public EditorModal
	{
	public:
		SettingsModal();

		void onImGuiRender() override;
		void onComplete() override;

		static Ref<SettingsModal> Create() { return Ref<SettingsModal>::Create(); }

	private:
		ApplicationSpec mSettings;
	};

}
