#include "Lpch.h"
#include "ModalManager.h"

#include <imgui.h>

namespace Laby {

	void ModalManager::Update(Timestep ts)
	{
		for (ModalEntry& modalData : sEditorModals)
		{
			if (modalData.modal && modalData.open)
				modalData.modal->onUpdate(ts);
		}
	}

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
        for (auto it = removeStart; it != sEditorModals.end(); it++)
        {
            if (sModalCallbacks.count(it->heading) == 0) continue;

            for (auto func : sModalCallbacks[it->heading])
                func();

            sModalCallbacks.erase(it->heading);
        }
        sEditorModals.erase(removeStart, sEditorModals.end());
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

	void ModalManager::DispatchEvents(Event& e)
	{
        for (auto& modalEntry : sEditorModals)
        {
            if (modalEntry.modal)
                modalEntry.modal->onEvent(e);
        }
	}


}
