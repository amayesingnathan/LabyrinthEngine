#pragma once

#include <Labyrinth/Containers/Grid.h>
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

			GridPosition pos() const { return { MinX(), MinY() }; }
			glm::vec2 size() const { return { MaxX() - MinX() + 1, MaxY() - MinY() + 1 }; }

			bool contains(const GridPosition& pos);
		};

	public:
		SubTexturePickerModal(AssetHandle& returnSheet, AssetHandle sheetInProgress);

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		AssetHandle* mReturnSheet;
		Ref<Texture2DSheet> mCurrentSheet;

		Grid<bool> mTextureSelections;
		NewTexture mCurrentlyAdding;

		std::string mErrorText;
	};

}
