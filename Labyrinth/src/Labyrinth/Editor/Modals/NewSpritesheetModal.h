#pragma once

#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class NewSpritesheetModal : public IEditorModal
	{
	public:
		NewSpritesheetModal(AssetHandle& returnSheet, const fs::path& filepath);

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		AssetHandle* mReturnSheet;
		fs::path mTexturePath;

		AssetHandle mInProgressSheet = 0;

		std::string mSheetName;
		u32 mTileWidth = 0, mTileHeight = 0;
	};

}
