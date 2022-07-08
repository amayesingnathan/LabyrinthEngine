#pragma once

#include "PanelManager.h"

#include "Labyrinth/Renderer/Texture.h"

#include <filesystem>

namespace Labyrinth {

	class ContentBrowserPanel : public IPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender() override;

		static Ref<ContentBrowserPanel> Create() { return CreateRef<ContentBrowserPanel>(); }

	private:
		std::filesystem::path mCurrentDirectory; 
		
		Ref<Texture2D> mDirectoryIcon;
		Ref<Texture2D> mFileIcon;
	};

}