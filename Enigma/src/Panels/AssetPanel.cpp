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
			ImGui::PushID(key.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)(intptr_t)mAssetIcon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("ASSET_MANAGER_ITEM", &key, sizeof(std::string));
				ImGui::EndDragDropSource();
			}

			if (AssetManager::IsGroup(asset) && ImGui::BeginPopupContextItem())
			{
				AssetGroupVariant group = AssetManager::GetGroup(asset);

				std::visit([](auto& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					if constexpr (!(std::is_same_v<T, Ref<Tex2DGroup>> || std::is_same_v<T, Ref<SubTex2DGroup>> || std::is_same_v<T, Ref<Tex2DSheetGroup>>))
						LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
					
					for (const auto& [key, tex] : *arg)
						ImGui::MenuItem(key.c_str());
					
				}, group);
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();

			size_t refCount = AssetManager::GetRefCount(asset);

			std::string assetLabel = key + " - " + std::to_string(refCount);

			ImGui::TextWrapped(assetLabel.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		// TODO: status bar
		ImGui::End();
	}

}