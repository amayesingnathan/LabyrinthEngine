#pragma once

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/EditorPanel.h>

namespace Labyrinth {

	class TilemapPanel : public EditorPanel
	{
	public:
		TilemapPanel() = default;

		void onImGuiRender() override;

		static Ref<TilemapPanel> Create() { return Ref<TilemapPanel>::Create(); }

	private:
		AssetHandle mSelected;
	};

}