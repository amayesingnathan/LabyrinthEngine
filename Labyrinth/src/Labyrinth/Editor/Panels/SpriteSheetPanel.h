#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

    class SpriteSheetPanel : public IEditorPanel
    {
    public:
        SpriteSheetPanel() = default;

        void onImGuiRender() override;

    private:
        Ref<Texture2DSheet> mCurrentSheet = nullptr;
        Ref<SubTexture2D> mCurrentSubTex = nullptr;

        std::string mNewSheetName;
        usize mNewSheetWidth, mNewSheetHeight;
    };

}