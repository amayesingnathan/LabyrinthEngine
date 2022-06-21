#pragma once

#include "PanelManager.h"

#include "Labyrinth/Renderer/Texture.h"

#include <filesystem>

namespace Labyrinth {

	class ContentBrowserPanel : public Panel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender();

	private:
		std::filesystem::path mCurrentDirectory; 
		
		Ref<Texture2D> mDirectoryIcon;
		Ref<Texture2D> mFileIcon;
	};

}