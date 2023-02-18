#include "Lpch.h"
#include "ModalManager.h"

#include <Labyrinth/ImGui/ImGuiCpp.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	void ModalManager::Render()
	{
        Utils::SetWindowMoveFromTitleBar();
        for (ModalEntry& modalData : sEditorModals)
        {
            if (!modalData.modal)
            {
                modalData.open = false;
                continue;
            }

            Utils::SetNextWindowSize<glm::vec2>({ 600, 400 });
            Utils::SetNextWindowPos<glm::vec2>(Utils::GetMainWindowCentre<glm::vec2>(), { 0.5f, 0.5f });
            if (Widgets::BeginWindow(modalData.heading, &modalData.open, 0))//ImGuiWindowFlags_NoDocking))
            {
                modalData.modal->onImGuiRender();
                RenderButtons(modalData);
            }

            Widgets::EndWindow();
        }
        Utils::SetWindowMoveFromTitleBar(false);

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

        Widgets::NewLine();
        Widgets::Separator();

        switch (modalData.type)
        {
        case ModalButtons::OK:
        {
            Widgets::Button("OK", [&]()
            {
                modalData.modal->onComplete();
                modalData.open = false;
            });
            break;
        }
        case ModalButtons::OKCancel:
        {
            Widgets::Button("OK", [&]()
            {
                modalData.modal->onComplete();
                modalData.open = false;
            });

            Widgets::SameLine();

            Widgets::Button("Cancel", [&]() { modalData.open = false; });

            break;
        }
        case ModalButtons::YesNo:
        {
            Widgets::Button("Yes", [&]()
            {
                modalData.modal->onComplete();
                modalData.open = false;
            });

            Widgets::SameLine();

            Widgets::Button("No", [&]() { modalData.open = false; });

            break;
        }
        case ModalButtons::Custom:
            modalData.modal->onCustomButtonRender(modalData.open);
            break;
        }
	}
}
