#include "Lpch.h"
#include "RenderThread.h"

#include <Labyrinth/Renderer/Camera.h>

namespace Laby{

	void RenderThread::Init()
    {
        sThreadState = MakeSingle<RenderThreadState>();
    }

    void RenderThread::Start()
    {
        sThreadState->complete = false;
        sThreadState->thread = std::thread(DoWork);
    }

    void RenderThread::Wait()
    {
        sThreadState->complete = true;
        sThreadState->completionVar.notify_one();
        sThreadState->thread.join();
        std::swap(sThreadState->queuedWork, sThreadState->executingWork);
    }

    UUID RenderThread::AddWaitCallback(Action<>&& callback)
    {
        std::scoped_lock lock(sThreadState->callbackMutex);
        return sThreadState->waitCallbacks.emplace_back(std::move(callback)).id;
    }

    void RenderThread::RemoveWaitCallback(UUID id)
    {
        std::scoped_lock lock(sThreadState->callbackMutex);
        std::erase_if(sThreadState->waitCallbacks, [id](const WaitCallback& callback) { return callback.id == id; });
    }

    void RenderThread::Submit(Action<>&& renderWork)
    {
        sThreadState->queuedWork.emplace_back(std::move(renderWork));
    }

    void RenderThread::DoWork()
    {
        for (const Action<>& job : sThreadState->executingWork)
            job();

        sThreadState->executingWork.clear();

        std::unique_lock lock(sThreadState->completionMutex);
        sThreadState->completionVar.wait(lock, [&] { return sThreadState->complete.load(); });
        for (const WaitCallback& callback : sThreadState->waitCallbacks)
            callback.work();
    }
}