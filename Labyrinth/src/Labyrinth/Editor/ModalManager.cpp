#include "Lpch.h"
#include "ModalManager.h"

#include <imgui.h>

namespace Labyrinth {

    void ModalManager::Display()
    {
        for (ModalEntry& modalData : sModals)
        {
            // Do this here in case instead of Open() as it will fail if Open() is called midway through another popup.
            if (modalData.begin)
            {
                ImGui::OpenPopup(modalData.heading.c_str());
                modalData.begin = false;
            }

            ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (!modalData.modal->isComplete() && ImGui::BeginPopupModal(modalData.heading.c_str(), nullptr, modalData.flags))
            {
                modalData.modal->onImGuiRender();

                RenderButtons(modalData);
                ImGui::EndPopup();
            }
        }

        // Call any completion callbacks before deleting modal entries
        auto removeStart = std::remove_if(sModals.begin(), sModals.end(), [](const ModalEntry& entry) { return entry.modal->isComplete(); });
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
                modalData.modal->onClose();
            }
            break;
        }
        case ModalButtons::OKCancel:
        {
            if (ImGui::Button("OK"))
            {
                modalData.modal->onComplete();
                modalData.modal->onClose();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                modalData.modal->onClose();

            break;
        }
        case ModalButtons::YesNo:
        {
            if (ImGui::Button("Yes"))
            {
                modalData.modal->onComplete();
                modalData.modal->onClose();
            }

            ImGui::SameLine();

            if (ImGui::Button("No"))
                modalData.modal->onClose();

            break;
        }
        case ModalButtons::Custom:
            modalData.modal->onCustomButtonRender();
            break;
        }
    }
}