#pragma once

#include "task.h"

namespace Engine {
    namespace Threading{

struct MadgineObjectState {

    Threading::TaskFuture<bool> state()
    {
        return mState.ensure();
    }

    bool isInitialized()
    {
        Threading::TaskFuture<bool> state = mState.load();
        return state.valid() && state.is_ready() && state;
    }

protected:
    ~MadgineObjectState()
    {
        Threading::TaskFuture<bool> state = mState.load();
        if (state.valid()) {
            if (!state.attached())
                LOG_WARNING("Unattached TaskFuture in " << mTypeInfo->name() << ". Some coroutine is probably waiting for initialization which will never happen.");
            else
                LOG_WARNING("Deleting still initialized Object: " << mTypeInfo->name());
        }
    }

protected:
    Threading::AtomicTaskFuture<bool> mState;
    const std::type_info *mTypeInfo = nullptr;
};

template <typename T>
struct MadgineObject : MadgineObjectState {
    Threading::Task<bool> callInit()
    {
        assert(!mState.load().attached());
        mTypeInfo = &typeid(static_cast<T &>(*this));
        LOG("Initializing: " << mTypeInfo->name() << "...");
        auto task = Threading::make_task(&T::init, static_cast<T *>(this));
        task.set_future(state());

        return task;
    }

    Threading::Task<void> callFinalize()
    {
        assert(mState.load());
        mState.reset();
        if constexpr (Threading::IsTask<decltype(static_cast<T *>(this)->finalize())>) {
            co_await static_cast<T *>(this)->finalize();
        } else {
            static_cast<T *>(this)->finalize();
        }
        co_return;
    }
};

    }
}
