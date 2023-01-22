#include "Lpch.h"
#include "ContentBrowserPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		mAssetDirectory = "assets";
		if (Project::IsActive())
		{
			mCurrentDirectory = Project::GetAssetDirectory();;
			mAssetDirectory = Project::GetAssetDirectory();;
		}
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		if (!Project::IsActive())
			return;

		Widgets::Button("<-", [this]()
		{
			if (mCurrentDirectory != mAssetDirectory)
			mCurrentDirectory = mCurrentDirectory.parent_path();
		});

		float cellSize = mThumbnailSize + mPadding;
		
		float panelWidth = ImGuiUtils::AvailableRegion().x;
		i32 columnCount = (i32)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		Widgets::BeginColumns(columnCount);
		for (auto& dirEntry : fs::directory_iterator(mCurrentDirectory))
		{
			fs::path dirPath = dirEntry.path();
			fs::path relativePath = fs::relative(dirPath, mAssetDirectory);
			if (relativePath.extension() == ".lreg")
				continue;

			bool isDir = dirEntry.is_directory();
			Ref<Texture2D> icon = isDir ? EditorResources::DirectoryIcon : EditorResources::FileIcon;

			fs::path filename = dirEntry.path().filename();
			std::string filenameStr = filename.string();
			ImGuiUtils::PushID(filenameStr);
			ImGuiUtils::PushStyleColour(21, glm::vec4{ 0.0f }); // ImGuiCol_Button

			Widgets::ImageButton(icon, { mThumbnailSize, mThumbnailSize });
			Widgets::AddDragDropSource("CONTENT_BROWSER_ITEM", relativePath);

			ImGuiUtils::PopStyleColour();

			Widgets::OnWidgetSelected([&]()
			{
				if (isDir)
					mCurrentDirectory /= filename;
			});
			Widgets::LabelWrapped(filenameStr);

			Widgets::NextColumn();
			ImGuiUtils::PopID();
		}

		Widgets::EndColumns();

		Widgets::FloatEdit("Thumbnail Size", mThumbnailSize, 1.0f, 16, 512);
		Widgets::FloatEdit("Padding", mPadding, 1.0f, 0, 32);
	}

	void ContentBrowserPanel::onProjectChange()
	{
		if (!Project::IsActive())
			return;

		mCurrentDirectory = Project::GetAssetDirectory();
		mAssetDirectory = Project::GetAssetDirectory();
	}
}