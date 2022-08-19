#pragma once

#include <Labyrinth/Core/Application.h>

namespace Labyrinth {

    enum class ModalButtons
    {
        None, OK, OKCancel, YesNo, Custom
    };

    class EditorModal : public RefCounted
    {
    protected:
        EditorModal() { Application::BlockEsc(); }

    protected:
        virtual void onImGuiRender() = 0;
        virtual void onComplete() {}
        virtual void onCustomButtonRender() { LAB_CORE_ASSERT(false, "You must provide an override for this function if using custom button behaviour!"); }

        void onClose();

        bool isComplete() const { return mComplete; }


    private:
        bool mComplete = false;

        friend class ModalManager;
        friend class InlineModal;
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

    class InlineModal : public EditorModal
    {
    public:
        InlineModal(std::function<void()> onImGuiRenderLambda, std::function<void()> onCompleteLambda)
            : mOnImGuiRender(onImGuiRenderLambda), mOnComplete(onCompleteLambda)
        {}

        void onImGuiRender() override { mOnImGuiRender(); }
        void onComplete() override { mOnComplete(); }

        static Ref<InlineModal> Create(std::function<void()> onImGuiRenderLambda, std::function<void()> onCompleteLambda) { return Ref<InlineModal>::Create(onImGuiRenderLambda, onCompleteLambda); }

    private:
        std::function<void()> mOnImGuiRender, mOnComplete;
    };
}