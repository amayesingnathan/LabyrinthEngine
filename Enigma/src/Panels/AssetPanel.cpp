#include "AssetPanel.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	AssetPanel::AssetPanel()
	{
		Ref<Tex2DGroup> iconGroup = AssetManager::Get<Tex2DGroup>("Icons");
		mAssetIcon = iconGroup->addOrGet("File", "resources/icons/content-browser/fileIcon.png");
	}

	void AssetPanel::onImGuiRender()
	{
		ImGui::Begin("Asset Manager");

		static float padding = 12.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (const auto& [key, asset] : AssetManager::GetAssets())
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)(intptr_t)mAssetIcon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("ASSET_MANAGER_ITEM", &key, sizeof(std::string));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();

			std::string assetLabel = key + " - " + std::to_string(asset.use_count());

			ImGui::TextWrapped(assetLabel.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		// TODO: status bar
		ImGui::End();
	}

}