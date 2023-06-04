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
        AssetHandle mCurrentSheet = 0;
        AssetHandle mCurrentSubTex = 0;

        std::string mNewSheetName;
        usize mNewSheetWidth, mNewSheetHeight;
    };

}