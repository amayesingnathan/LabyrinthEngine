#pragma once

#include "PanelManager.h"

#include "Labyrinth/Renderer/Texture.h"

namespace Labyrinth {

	class AssetPanel : public Panel
	{
	public:
		AssetPanel();

		void onImGuiRender() override;

		static Ref<AssetPanel> Create() { return Ref<AssetPanel>::Create(); }

	};

}