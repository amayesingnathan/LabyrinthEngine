#include "ContentBrowserPanel.h"

#include <Labyrinth/Editor/EditorResources.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		mAssetDirectory = "assets";
		if (Project::GetActive())
		{
			mCurrentDirectory = Project::GetAssetDirectory();;
			mAssetDirectory = Project::GetAssetDirectory();;
		}
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (!Project::GetActive())
		{
			ImGui::End();
			return;
		}

		if (ImGui::Button("<-") && (mCurrentDirectory != mAssetDirectory))
			mCurrentDirectory = mCurrentDirectory.parent_path();

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		i32 columnCount = (i32)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& dirEntry : std::filesystem::directory_iterator(mCurrentDirectory))
		{
			auto relativePath = std::filesystem::relative(dirEntry.path(), mAssetDirectory);

			if (relativePath.extension().string() == ".lreg")
				continue;

			const auto& filepath = dirEntry.path().filename();
			std::string filenameStr = filepath.string();

			ImGui::PushID(filenameStr.c_str());
			Ref<Texture2D> icon = dirEntry.is_directory() ? EditorResources::DirectoryIcon : EditorResources::FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::ImageButton((ImTextureID)(intptr_t)icon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource())
			{
				const FS_CHAR_TYPE* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (FS_STR_LEN(itemPath) + 1) * sizeof(FS_CHAR_TYPE));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (dirEntry.is_directory())
					mCurrentDirectory /= filepath;
			}
			ImGui::TextWrapped(filenameStr.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}

	void ContentBrowserPanel::onProjectChange(const Ref<Project>& project)
	{
		if (project)
		{
			mCurrentDirectory = project->GetAssetDirectory();
			mAssetDirectory = project->GetAssetDirectory();
		}
	}

}