#pragma once

#include "EditorModal.h"

#include <Labyrinth/Core/UUID.h>

#include <vector>

namespace Labyrinth {

    class ModalManager
    {
    public:
        struct ModalEntry
        {
            UUID id;
            std::string heading;
            Ref<EditorModal> modal = nullptr;
            ModalButtons type = ModalButtons::OKCancel;

            bool open = true;
            i32 flags = 1 << 21; // ImGuiWindowFlags_NoDocking

            ModalEntry(const std::string& h, ModalButtons t, const Ref<EditorModal>& m) : heading(h), modal(m), type(t) {}
        };

        using ModalList = std::vector<ModalEntry>;
        using CallbackMap = std::unordered_map<UUID, std::vector<std::function<void()>>>;

    public:
        template<typename T, typename... Args>
        static void Open(const std::string& title, ModalButtons type, Args&&... args)
        {
            if (Exists(title))
            {
                LAB_CORE_WARN("Modal already open!");
                return;
            }

            sNewModal = sModals.emplace_back(title, type, T::Create(std::forward<Args>(args)...)).id;
        }
        static void Open(const std::string& title, ModalButtons type, std::function<void()> onImGuiRender, std::function<void()> onComplete = std::function<void()>())
        {
            Open<InlineModal>(title, type, onImGuiRender, onComplete);
        }
        static void OpenWarning(const std::string& title, const std::string& msg)
        {
            Open<WarningModal>(title, ModalButtons::OK, msg);
        }

        static void AddOnComplete(std::function<void()> function)
        {
            sCallbacks[sNewModal].push_back(function);
        }

    private:
        static bool Exists(const std::string& title)
        {
            return std::find_if(sModals.begin(), sModals.end(), [&](const ModalEntry& entry) { return title == entry.heading; }) != sModals.end();
        }

    private:
        static void Render();
        static void RenderButtons(ModalEntry& modalData);
        static void DispatchEvents(Event& e);

    private:
        inline static ModalList sModals;
        inline static CallbackMap sCallbacks;
        inline static UUID sNewModal;

        friend class EditorLayer;
    };
}