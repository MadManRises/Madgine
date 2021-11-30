#pragma once

#if ENABLE_THREADING

#    include "workgroup.h"

namespace Engine {
namespace Threading {

    struct WorkGroupHandle {
        WorkGroupHandle() = default;
        WorkGroupHandle(const WorkGroupHandle &) = delete;

        template <typename F, typename... Args>
        WorkGroupHandle(F &&main, Args &&... args)
        {
            std::promise<int> p;
            mResult = p.get_future();
            mThread = std::thread(&WorkGroupHandle::threadMain<F, Args...>, std::move(p), std::forward<F>(main), std::forward<Args>(args)...);
        }

        ~WorkGroupHandle()
        {
            if (mThread.joinable()) {
                mThread.join();
                mResult.get();
            }
        }

        WorkGroupHandle &operator=(WorkGroupHandle &&other)
        {
            assert(!mThread.joinable());
            mResult = std::move(other.mResult);
            mThread = std::move(other.mThread);
            return *this;
        }

        void detach()
        {
            assert(mThread.joinable());
            mThread.detach();
        }

    private:
        template <typename F, typename... Args>
        static void threadMain(std::promise<int> p, F &&main, Args &&... args)
        {
            WorkGroup group;
            try {
                int result = TupleUnpacker::invokeDefaultResult(0, std::forward<F>(main), std::forward<Args>(args)...);
                p.set_value(result);
            } catch (std::exception &e) {
                LOG_ERROR("Uncaught Exception in Workgroup-MainThread!");
                LOG_ERROR(e.what());
                throw;
            }
        }

    private:
        std::future<int> mResult;
        std::thread mThread;
    };

}
}

#endif