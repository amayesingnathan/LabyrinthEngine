#pragma once

#include <Labyrinth/Editor/EditorPanel.h>
#include <Labyrinth/Renderer/Texture.h>

#include <filesystem>

namespace Labyrinth {

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender() override;
		void onProjectChange(const Ref<Project>& project) override;

		static Ref<ContentBrowserPanel> Create() { return Ref<ContentBrowserPanel>::Create(); }

	private:
		fs::path mCurrentDirectory;
		fs::path mAssetDirectory;
	};

}