#pragma once

#include <Labyrinth.h>

#include "Modal.h"

#include <imgui/imgui.h>

namespace Labyrinth {

	class SettingsModal : public Modal
	{
	public:
		struct StartupSettings
		{
			bool fullscreen = false;
		};

	public:
		SettingsModal();

		void onImGuiRender() override;

		static Ref<SettingsModal> Create() { return CreateRef<SettingsModal>(); }

	private:
		void Save();

	private:
		StartupSettings mSettings;
		JsonObj mSettingsJSON;
	};

}
