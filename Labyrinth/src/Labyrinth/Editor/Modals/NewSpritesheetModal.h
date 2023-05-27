#pragma once

#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class NewSpritesheetModal : public IEditorModal
	{
	public:
		NewSpritesheetModal(Ref<Texture2DSheet>& context, const fs::path& filepath);

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		Ref<Texture2DSheet>& mCurrentSheet;
		const fs::path& mTexturePath;

		std::string mSheetName;
		u32 mTileWidth = 0, mTileHeight = 0;
	};

}
