#pragma once

#include "PanelManager.h"

#include "Labyrinth/Renderer/Texture.h"

namespace Labyrinth {

	class AssetPanel : public IPanel
	{
	public:
		AssetPanel();

		void onImGuiRender() override;

		static Ref<AssetPanel> Create() { return CreateRef<AssetPanel>(); }

	private:
		Ref<Texture2D> mAssetIcon, mGroupIcon;
	};

}