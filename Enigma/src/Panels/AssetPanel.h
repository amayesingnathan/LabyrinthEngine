#pragma once

#include "PanelManager.h"

namespace Labyrinth {

	class AssetPanel : public Panel
	{
	public:
		void onImGuiRender() override;
	};

}