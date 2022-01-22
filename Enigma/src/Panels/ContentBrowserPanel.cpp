#include "Lpch.h"
#include "ContentBrowserPanel.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	// Once we're loading projects, change this
	static const std::filesystem::path sAssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: mCurrentDirectory(sAssetPath) {}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (ImGui::Button("<-"))
			if (mCurrentDirectory != sAssetPath)
				mCurrentDirectory = mCurrentDirectory.parent_path();

		for (auto& dirEntry : std::filesystem::directory_iterator(mCurrentDirectory))
		{
			const auto& filepath = dirEntry.path().filename();
			std::string filepathStr = filepath.string();
			if (dirEntry.is_directory())
			{
				if (ImGui::Button(filepathStr.c_str()))
					mCurrentDirectory /= filepath;
			}
			else
			{
				ImGui::Text("%s", filepathStr.c_str());
			}
		}

		ImGui::End();
	}

}