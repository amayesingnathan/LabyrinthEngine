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
            ModalType type = ModalType::OKCancel;

            bool begin = true;
            i32 flags = 0;

            ModalEntry(const std::string& h, ModalType t, const Ref<EditorModal>& m) : heading(h), modal(m), type(t) {}
        };

        using ModalArray = std::vector<ModalEntry>;
        using CallbackMap = std::unordered_map<UUID, std::vector<std::function<void()>>>;

    private:
        inline static ModalArray sModals;
        inline static CallbackMap sCallbacks;
        inline static UUID sNewModal;

        friend class EditorLayer;

    public:
        template<typename T, typename... Args>
        static void Open(const std::string& name, ModalType type, Args&&... args)
        {
            sNewModal = sModals.emplace_back(name, type, T::Create(std::forward<Args>(args)...)).id;
        }
        static void AddOnComplete(std::function<void()> function)
        {
            sCallbacks[sNewModal].push_back(function);
        }

        static void OpenWarning(const std::string& title, const std::string& msg)
        {
            Open<WarningModal>(title, ModalType::OK, msg);
        }

    private:
        static void Display();
        static void RenderButtons(ModalEntry& modalData);
    };
}