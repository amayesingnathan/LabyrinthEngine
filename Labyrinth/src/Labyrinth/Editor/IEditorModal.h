#pragma once

#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/Event.h"
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
        virtual void OnRender() = 0;
        virtual void OnComplete() {}
        virtual void OnCustomButtonRender(bool& open) { LAB_CORE_ASSERT(false, "You must provide an override for this function if using custom button behaviour!"); }

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

        void OnRender() override;

    private:
        std::string mMessage;
    };

    class InlineModal : public IEditorModal
    {
    public:
        InlineModal(Action<>&& OnRenderLambda, Action<>&& OnCompleteLambda)
            : mOnRender(std::move(OnRenderLambda)), mOnComplete(std::move(OnCompleteLambda))
        {}

        void OnRender() override { mOnRender(); }
        void OnComplete() override { mOnComplete(); }

    private:
        Action<> mOnRender, mOnComplete;
    };
}