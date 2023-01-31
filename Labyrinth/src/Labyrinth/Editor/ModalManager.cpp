#include "Lpch.h"
#include "ModalManager.h"

#include <imgui.h>

namespace Laby {

	void ModalManager::Render()
	{
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        for (ModalEntry& modalData : sEditorModals)
        {
            if (!modalData.modal)
            {
                modalData.open = false;
                continue;
            }

            ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(centre, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            if (ImGui::Begin(modalData.heading.data(), &modalData.open, ImGuiWindowFlags_NoDocking))
            {
                modalData.modal->onImGuiRender();
                RenderButtons(modalData);
            }

            ImGui::End();
        }
        io.ConfigWindowsMoveFromTitleBarOnly = false;

        // Call any completion callbacks before deleting modal entries
        auto removeStart = std::remove_if(sEditorModals.begin(), sEditorModals.end(), [](const ModalEntry& entry) { return !entry.open; });

        auto toRemove = sEditorModals |
            std::views::filter([](const ModalEntry& entry) { return entry.open && sModalCallbacks.contains(entry.heading); });

        for (const ModalEntry& entry : toRemove)
        {
            for (auto func : sModalCallbacks[entry.heading])
                func();

            sModalCallbacks.erase(entry.heading);
        }

        std::erase_if(sEditorModals, [](const ModalEntry& entry) { return !entry.open; });
	}

	void ModalManager::RenderButtons(ModalEntry& modalData)
	{
        if (modalData.type == ModalButtons::None) 
            return;

        ImGui::NewLine();
        ImGui::Separator();

        switch (modalData.type)
        {
        case ModalButtons::OK:
        {
            if (ImGui::Button("OK"))
            {
                modalData.modal->onComplete();
                modalData.open = false;
            }
            break;
        }
        case ModalButtons::OKCancel:
        {
            if (ImGui::Button("OK"))
            {
                modalData.modal->onComplete();
                modalData.open = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                modalData.open = false;

            break;
        }
        case ModalButtons::YesNo:
        {
            if (ImGui::Button("Yes"))
            {
                modalData.modal->onComplete();
                modalData.open = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("No"))
                modalData.open = false;

            break;
        }
        case ModalButtons::Custom:
            modalData.modal->onCustomButtonRender(modalData.open);
            break;
        }
	}
}
