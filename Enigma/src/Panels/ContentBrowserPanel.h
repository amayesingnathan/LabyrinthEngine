#pragma once

#include <filesystem>

namespace Labyrinth {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender();

	private:
		std::filesystem::path mCurrentDirectory;
	};

}