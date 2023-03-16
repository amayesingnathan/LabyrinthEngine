#pragma once

#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/IEventListener.h"
#include "Labyrinth/Project/Project.h"

namespace Laby {

    enum class ModalButtons
    {
        None, OK, OKCancel, YesNo, Custom
    };

    class IEditorModal : public RefCounted
    {
    public:
        IEditorModal();
        virtual ~IEditorModal();

    protected:
        virtual void onImGuiRender() = 0;
        virtual void onComplete() {}
        virtual void onCustomButtonRender(bool& open) { LAB_CORE_ASSERT(false, "You must provide an override for this function if using custom button behaviour!"); }

    private:
        friend class ModalManager;
    };

    template<typename T>
    concept IsEditorModal = DerivedFrom<IEditorModal, T>;

    class WarningModal : public IEditorModal
    {
    public:
        WarningModal(const std::string& msg)
            : IEditorModal(), mMessage(msg) {}

        void onImGuiRender() override;

    private:
        std::string mMessage;
    };

    class InlineModal : public IEditorModal
    {
    public:
        InlineModal(Action<>&& onImGuiRenderLambda, Action<>&& onCompleteLambda)
            : mOnImGuiRender(std::move(onImGuiRenderLambda)), mOnComplete(std::move(onCompleteLambda))
        {}

        void onImGuiRender() override { mOnImGuiRender(); }
        void onComplete() override { mOnComplete(); }

    private:
        Action<> mOnImGuiRender, mOnComplete;
    };
}