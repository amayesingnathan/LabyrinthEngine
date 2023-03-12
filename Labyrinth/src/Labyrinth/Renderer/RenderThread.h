#pragma once

#include <Labyrinth/Core/UUID.h>
#include <Labyrinth/Renderer/Renderer2D.h>

namespace Laby {

    struct WaitCallback
    {
        UUID id;
        Action<> work;

        WaitCallback(Action<>&& callback)
            : work(std::move(callback)) {}
    };

    struct RenderThreadState
    {
        std::thread thread;
        std::mutex completionMutex, callbackMutex;
        std::atomic_bool complete = false;
        std::condition_variable completionVar;

        std::vector<Action<>> queuedWork;
        std::vector<Action<>> executingWork;
        std::vector<WaitCallback> waitCallbacks;
    };

    class RenderThread
    {
    private:
        RenderThread() = delete;

        static void Init();

        static void Start();
        static void Wait();

        static UUID AddWaitCallback(Action<>&& callback);
        static void RemoveWaitCallback(UUID id);

        static void Submit(Action<>&& renderWork);

        static void DoWork();

    private:
        inline static Single<RenderThreadState> sThreadState;
        
        friend class Renderer;
    };
}