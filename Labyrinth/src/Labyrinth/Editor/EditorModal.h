#pragma once

#include <Labyrinth/Core/Application.h>

namespace Labyrinth {

    enum class ModalType
    {
        OK, OKCancel, YesNo, Custom
    };

    class EditorModal : public RefCounted
    {
    protected:
        EditorModal() { Application::BlockEsc(); }

    private:
        virtual void onImGuiRender() = 0;
        virtual void onCustomButtonRender() { LAB_CORE_ASSERT(false, "You must provide an override for this function if using custom button behaviour!"); }
        virtual void onComplete() {}

        void onClose();

        bool isComplete() const { return mComplete; }


    private:
        bool mComplete = false;

        friend class ModalManager;
    };

    class WarningModal : public EditorModal
    {
    public:
        WarningModal(const std::string& msg)
            : EditorModal(), mMessage(msg) {}

        void onImGuiRender() override;

        static Ref<WarningModal> Create(const std::string& msg) { return Ref<WarningModal>::Create(msg); }

    private:
        std::string mMessage;
    };
}