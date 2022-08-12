#pragma once

#include <Labyrinth/Editor/EditorPanel.h>
#include <Labyrinth/Renderer/Texture.h>

namespace Labyrinth {

	class AssetPanel : public EditorPanel
	{
	public:
		AssetPanel();

		void onImGuiRender() override;

		static Ref<AssetPanel> Create() { return Ref<AssetPanel>::Create(); }

	};

}