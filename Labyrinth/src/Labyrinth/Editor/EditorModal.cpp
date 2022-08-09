#include "Lpch.h"
#include "EditorModal.h"

#include <imgui.h>

namespace Labyrinth {

	void WarningModal::onImGuiRender()
    {
        ImGui::Text(mMessage.c_str());
    }

    void EditorModal::onClose()
    {
        mComplete = true;

        ImGui::CloseCurrentPopup();
        Application::BlockEsc(false);
    }


}