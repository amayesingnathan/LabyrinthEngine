#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>
#include <Labyrinth/Renderer/Texture.h>

#include <filesystem>

namespace Laby{

	class ContentBrowserPanel : public IEditorPanel
	{
	public:
		ContentBrowserPanel();

		void OnRender() override;
		void OnProjectChange() override;

	private:
		fs::path mCurrentDirectory;
		fs::path mAssetDirectory;

		f32 mPadding = 16.0f;
		f32 mThumbnailSize = 128.0f;
	};

}