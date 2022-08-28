#pragma once

#include <Labyrinth/Core/Application.h>

namespace Labyrinth {

    enum class ModalButtons
    {
        None, OK, OKCancel, YesNo, Custom
    };

    class EditorModal : public RefCounted
    {
    public:
        EditorModal() { Application::BlockEsc(); }
        virtual ~EditorModal() { Application::BlockEsc(false); }

    protected:
        virtual void onUpdate(Timestep ts) {}
        virtual void onImGuiRender() = 0;
        virtual void onComplete() {}
        virtual void onCustomButtonRender(bool& open) { LAB_CORE_ASSERT(false, "You must provide an override for this function if using custom button behaviour!"); }
        virtual void onEvent(Event& e) {}

    private:
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