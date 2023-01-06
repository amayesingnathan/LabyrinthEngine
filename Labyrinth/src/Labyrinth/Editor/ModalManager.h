#pragma once

#include "IEditorModal.h"

namespace Laby {


    class ModalManager
    {
	private:
        struct ModalEntry
        {
            std::string_view heading;
            Ref<IEditorModal> modal = nullptr;
            ModalButtons type = ModalButtons::OKCancel;

            bool open = true;

            ModalEntry(std::string_view h, ModalButtons t, Ref<IEditorModal> m) : heading(h), modal(m), type(t) {}
        };
    public:
        template<IsEditorModal T, typename... Args>
        static void Open(std::string_view title, ModalButtons type, Args&&... args)
        {
            if (Contains(title))
            {
                LAB_CORE_WARN("Modal already open!");
                return;
            }

            sLastAdded = sEditorModals.emplace_back(title, type, Ref<T>::Create(std::forward<Args>(args)...)).heading;
        }

        static void OpenInline(std::string_view title, ModalButtons type, std::function<void()> onImGuiRender, std::function<void()> onComplete = std::function<void()>()) { Open<InlineModal>(title, type, onImGuiRender, onComplete); }
        static void OpenWarning(std::string_view title, const std::string& msg) { Open<WarningModal>(title, ModalButtons::OK, msg); }

        static void AddCallback(std::function<void()> function) { sModalCallbacks[sLastAdded].emplace_back(function); }

    private:
        static void Update(Timestep ts);
        static void Render();
        static void RenderButtons(ModalEntry& modalData);
        static void DispatchEvents(Event& e);

        static bool Contains(std::string_view key) { return Find(key) != sEditorModals.end(); }
        static std::vector<ModalEntry>::iterator Find(std::string_view key) { return std::find_if(sEditorModals.begin(), sEditorModals.end(), [&key](const ModalEntry& panel) { return key == panel.heading; }); }

        static void Clear() { sEditorModals.clear(); }

    private:
        inline static std::vector<ModalEntry> sEditorModals;

        using CallbackMap = std::unordered_map<std::string_view, std::vector<std::function<void()>>>;
        inline static CallbackMap sModalCallbacks;
        inline static std::string_view sLastAdded;

        friend class EditorLayer;
    };
}