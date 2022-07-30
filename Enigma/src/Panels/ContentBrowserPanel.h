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

		static Ref<ContentBrowserPanel> Create() { return Ref<ContentBrowserPanel>::Create(); }

	private:
		std::filesystem::path mCurrentDirectory;
	};

}