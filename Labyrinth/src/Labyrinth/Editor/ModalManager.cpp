#include "Lpch.h"
#include "ModalManager.h"

#include <imgui.h>

namespace Labyrinth {

    void ModalManager::Display()
    {
        for (ModalEntry& modalData : sModals)
        {
            ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(centre, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            if (ImGui::Begin(modalData.heading.c_str(), &modalData.open, modalData.flags))
            {
                modalData.modal->onImGuiRender();
                RenderButtons(modalData);
            }

            ImGui::End();
        }

        // Call any completion callbacks before deleting modal entries
        auto removeStart = std::remove_if(sModals.begin(), sModals.end(), [](const ModalEntry& entry) { return !entry.open; });
        for (auto it = removeStart; it != sModals.end(); it++)
        {
            if (sCallbacks.count(it->id) == 0) continue;

            for (auto func : sCallbacks[it->id])
                func();

            sCallbacks.erase(it->id);
        }
        sModals.erase(removeStart, sModals.end());

    }

    void ModalManager::RenderButtons(ModalEntry& modalData)
    {
        if (modalData.type == ModalButtons::None) return;

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