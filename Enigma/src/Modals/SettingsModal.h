#pragma once

#include "Modal.h"

#include <Labyrinth/IO/JSON.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	class SettingsModal : public Modal
	{
	public:
		SettingsModal();

		void onImGuiRender() override;

		static Ref<SettingsModal> Create() { return Ref<SettingsModal>::Create(); }

	private:
		void Save();

	private:
		ApplicationSpec& mSettings;
		JsonObj mSettingsJSON;
	};

}
