#pragma once

#include <Labyrinth/Editor/IEditorModal.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class SubTexturePickerModal : public IEditorModal
	{
	public:
		struct NewTexture
		{
			std::vector<GridPosition> points;

			bool valid() const;
			u32 MinX() const;
			u32 MaxX() const;
			u32 MinY() const;
			u32 MaxY() const;

			bool contains(const GridPosition& pos);
		};

	public:
		SubTexturePickerModal(Ref<Texture2DSheet> context);

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		Ref<Texture2DSheet> mCurrentSheet;

		std::vector<NewTexture> mTexturesToAdd;
		NewTexture mCurrentlyAdding;
	};

}
