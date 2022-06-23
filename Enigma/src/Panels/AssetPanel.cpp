#include "AssetPanel.h"

#include "Labyrinth/Assets/AssetManager.h"

#include <imgui/imgui.h>

namespace Labyrinth {

	void AssetPanel::onImGuiRender()
	{
		ImGui::Begin("Asset Manager");

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& [key, asset] : AssetManager::GetAssets())
		{

		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}

}